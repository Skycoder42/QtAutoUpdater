#include "qchocolateyupdaterbackend.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtAutoUpdaterCore/AdminAuthoriser>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logChocoBackend, "qt.autoupdater.core.plugin.chocolatey.backend")

QChocolateyUpdaterBackend::QChocolateyUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

UpdaterBackend::Features QChocolateyUpdaterBackend::features() const
{
	return Feature::CheckUpdates |
			Feature::TriggerInstall;
}

void QChocolateyUpdaterBackend::checkForUpdates()
{
	if (_chocoProc->state() == QProcess::NotRunning)
		_chocoProc->start(QIODevice::ReadOnly);
}

void QChocolateyUpdaterBackend::abort(bool force)
{
	if (_chocoProc->state() != QProcess::NotRunning) {
		if (force)
			_chocoProc->kill();
		else
			_chocoProc->terminate();
	}
}

bool QChocolateyUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
	Q_UNUSED(infos)

	// try GUI first
	QStringList paths;
	if (auto mPaths = config()->value(QStringLiteral("guiPath")); mPaths)
		paths = mPaths->toString().split(QDir::listSeparator());
	auto program = QStandardPaths::findExecutable(QStringLiteral("ChocolateyGui"), paths);

	QStringList arguments;
	if (program.isEmpty()) {
		const auto extraArgs = config()->value(QStringLiteral("extraInstallArgs"));
		if (extraArgs)
			arguments.append(extraArgs->toStringList());
	} else {
		const auto extraArgs = config()->value(QStringLiteral("extraGuiInstallArgs"));
		if (extraArgs)
			arguments.append(extraArgs->toStringList());
	}

	// find out if the application needs to be run as admin
	if (config()->value(QStringLiteral("runAsAdmin"), true).toBool()) {
		if (track)
			qCWarning(logChocoBackend) << "Unable to track progress of application executed as root user!";
		const auto ok = AdminAuthoriser::executeAsAdmin(program,
														arguments,
														QDir::homePath());
		if (ok && track) { // invoke queued to make shure is emitted AFTER the start install signal in the updater
			QMetaObject::invokeMethod(this, "triggerInstallDone", Qt::QueuedConnection,
									  Q_ARG(bool, true));
		}
		return ok;
	} else {
		if (track) {
			auto proc = new QProcess{this};
			proc->setProgram(program);
			proc->setArguments(arguments);
			proc->setWorkingDirectory(QDir::homePath());
			proc->setProcessChannelMode(QProcess::ForwardedChannels);
			proc->setInputChannelMode(QProcess::ForwardedInputChannel);
			connect(proc, &QProcess::stateChanged,
					this, &QChocolateyUpdaterBackend::installerState);
			proc->start(QIODevice::ReadWrite);
			return true;
		} else {
			if (QProcess::startDetached(program, arguments, QDir::homePath()))
				return true;
			else {
				qCCritical(logChocoBackend) << "Failed to start" << program << "to install updates";
				return false;
			}
		}
	}
}

UpdateInstaller *QChocolateyUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QChocolateyUpdaterBackend::initialize()
{
	if (auto pConf = config()->value(QStringLiteral("packages")); pConf) {
		if (pConf->userType() == QMetaType::QStringList)
			_packages = pConf->toStringList();
		else
			_packages = pConf->toString().split(QLatin1Char(','));
	}
	if (_packages.isEmpty()) {
		qCCritical(logChocoBackend) << "Configuration for chocolatey must contain 'packages' with at least one package";
		return false;
	}


	QStringList paths;
	if (auto mPaths = config()->value(QStringLiteral("path")); mPaths)
		paths = mPaths->toString().split(QDir::listSeparator());

	const auto chocoExe = QStandardPaths::findExecutable(QStringLiteral("choco"), paths);
	if (chocoExe.isEmpty()) {
		qCCritical(logChocoBackend) << "Failed to find choco executable";
		return false;
	}

	auto argsVal = config()->value(QStringLiteral("extraCheckArgs"));
	QStringList args {
		QStringLiteral("outdated"),
		QStringLiteral("--no-color"),
		QStringLiteral("--confirm"),
		QStringLiteral("--no-progress"),
		QStringLiteral("--ignore-unfound")
	};
	if (argsVal) {
		if (argsVal->userType() == QMetaType::QStringList)
			args.append(argsVal->toStringList());
		else
			args.append(argsVal->toString().split(QLatin1Char(' ')));
	}


	_chocoProc = new QProcess{this};
	_chocoProc->setProgram(chocoExe);
	_chocoProc->setArguments(args);
	_chocoProc->setWorkingDirectory(QDir::homePath());
	_chocoProc->setProcessChannelMode(QProcess::ForwardedErrorChannel);

	connect(_chocoProc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
			this, &QChocolateyUpdaterBackend::updaterReady);
	connect(_chocoProc, &QProcess::errorOccurred,
			this, &QChocolateyUpdaterBackend::updaterError);

	return true;
}

void QChocolateyUpdaterBackend::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitStatus == QProcess::NormalExit) {
		switch (exitCode) {
		case 0:
		case 2:
			parseUpdates();
			break;
		default:
			qCCritical(logChocoBackend) << "choco finished with an unclean exit code:" << exitCode;
			emit checkDone(false);
			break;
		}
	} else
		emit checkDone(false);
	_chocoProc->close();
}

void QChocolateyUpdaterBackend::updaterError(QProcess::ProcessError procError)
{
	qCCritical(logChocoBackend) << "Chocolatey-Error:"
								<< qUtf8Printable(_chocoProc->errorString());
	if (procError == QProcess::FailedToStart) {
		emit checkDone(false);
		_chocoProc->close();
	}
}

void QChocolateyUpdaterBackend::installerState(QProcess::ProcessState state)
{
	if (state == QProcess::NotRunning) {
		auto proc = qobject_cast<QProcess*>(sender());
		if (proc->exitStatus() == QProcess::NormalExit &&
			proc->exitCode() == EXIT_SUCCESS)
			emit triggerInstallDone(true);
		else
			emit triggerInstallDone(false);
		proc->disconnect(this);
		proc->deleteLater();
	}
}

void QChocolateyUpdaterBackend::parseUpdates()
{
	QList<UpdateInfo> updates;
	while (!_chocoProc->atEnd()) {
		const auto line = _chocoProc->readLine().trimmed().split('|');
		if (line.size() != 4)
			continue;

		UpdateInfo info;
		info.setName(QString::fromUtf8(line[0]));
		if (!_packages.contains(info.name()))
			continue;
		info.setVersion(QVersionNumber::fromString(QString::fromUtf8(line[2])));
		info.setIdentifier(info.name());
		updates.append(info);
	}
	emit checkDone(true, updates);
}
