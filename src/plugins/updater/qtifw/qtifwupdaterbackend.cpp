#include "qtifwupdaterbackend.h"
#include <QtAutoUpdaterCore/AdminAuthoriser>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>
using namespace QtAutoUpdater;

QtIfwUpdaterBackend::QtIfwUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

UpdaterBackend::Features QtIfwUpdaterBackend::features() const
{
	return Feature::CheckUpdates |
#ifndef Q_OS_WIN
			Feature::ParallelInstall |
#endif
			Feature::TriggerInstall;
}

void QtIfwUpdaterBackend::checkForUpdates()
{
	if (_process->state() == QProcess::NotRunning)
		_process->start(QIODevice::ReadOnly);
}

void QtIfwUpdaterBackend::abort(bool force)
{
	if (_process->state() != QProcess::NotRunning) {
		if (force)
			_process->kill();
		else
			_process->terminate();
	}
}

bool QtIfwUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &, bool track)
{
	QStringList arguments {
		config()->value(QStringLiteral("silent"), false).toBool() ?
					QStringLiteral("--silentUpdate") :
					QStringLiteral("--updater")
	};
	const auto extraArgs = config()->value(QStringLiteral("extraInstallArgs"));
	if (extraArgs)
		arguments.append(extraArgs->toStringList());

	// find out if the application needs to be run as admin
	bool runAsAdmin;
	if (auto runAsAdminOpt = config()->value(QStringLiteral("runAsAdmin")); runAsAdminOpt)
		runAsAdmin = runAsAdminOpt->toBool();
	else
		runAsAdmin = AdminAuthoriser::needsAdminPermission(_process->program());

	if (runAsAdmin) {
		if (track)
			qCWarning(logCat()) << "Unable to track progress of application executed as root user!";
		const auto ok = AdminAuthoriser::executeAsAdmin(_process->program(),
														_process->arguments(),
														_process->workingDirectory());
		if (ok && track) { // invoke queued to make shure is emitted AFTER the start install signal in the updater
			QMetaObject::invokeMethod(this, "triggerInstallDone", Qt::QueuedConnection,
									  Q_ARG(bool, true));
		}
		return ok;
	} else {
		if (track) {
			auto proc = new QProcess{this};
			proc->setProgram(_process->program());
			proc->setArguments(arguments);
			proc->setWorkingDirectory(_process->workingDirectory());
			proc->setProcessChannelMode(QProcess::ForwardedChannels);
			proc->setInputChannelMode(QProcess::ForwardedInputChannel);
			connect(proc, &QProcess::stateChanged,
					this, &QtIfwUpdaterBackend::installerState);
			proc->start(QIODevice::ReadWrite);
			return true;
		} else {
			if (QProcess::startDetached(_process->program(), arguments, _process->workingDirectory()))
				return true;
			else {
				qCCritical(logCat()) << "Failed to start" << _process->program() << "to install updates";
				return false;
			}
		}
	}
}

UpdateInstaller *QtIfwUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QtIfwUpdaterBackend::initialize()
{
	auto mtInfo = findMaintenanceTool();
	if (!mtInfo) {
		qCCritical(logCat()) << "Path to maintenancetool could not be determined or does not exist. "
								"Use the 'path' configuration parameter to explicitly specify it";
		return false;
	}

	_process = new QProcess{this};
	_process->setProgram(mtInfo->absoluteFilePath());
	_process->setWorkingDirectory(mtInfo->absolutePath());
	const auto extraArgs = config()->value(QStringLiteral("extraCheckArgs"));
	if (extraArgs)
		_process->setArguments(QStringList{QStringLiteral("--checkupdates")} + extraArgs->toStringList());
	else
		_process->setArguments({QStringLiteral("--checkupdates")});
	_process->setProcessChannelMode(QProcess::ForwardedErrorChannel);

	connect(_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
			this, &QtIfwUpdaterBackend::updaterReady);
	connect(_process, &QProcess::errorOccurred,
			this, &QtIfwUpdaterBackend::updaterError);

	return true;
}

void QtIfwUpdaterBackend::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitStatus == QProcess::NormalExit) {
		if (exitCode == EXIT_SUCCESS) {
			auto updates = parseUpdates();
			if (updates)
				emit checkDone(true, *updates);
			else
				emit checkDone(false);
		} else
			emit checkDone(true);
	} else
		emit checkDone(false);
	_process->close();
}

void QtIfwUpdaterBackend::updaterError(QProcess::ProcessError procError)
{
	qCCritical(logCat()) << "Maintenancetool-Error:"
						 << qUtf8Printable(_process->errorString());
	if (procError == QProcess::FailedToStart) {
		emit checkDone(false);
		_process->close();
	}
}

void QtIfwUpdaterBackend::installerState(QProcess::ProcessState state)
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

std::optional<QFileInfo> QtIfwUpdaterBackend::findMaintenanceTool()
{
#ifdef Q_OS_OSX
		auto path = QStringLiteral("../../maintenancetool");
#else
		auto path =  QStringLiteral("./maintenancetool");
#endif
	path = config()->value(QStringLiteral("path"), path).toString();

#if defined(Q_OS_WIN32)
	if(!path.endsWith(QStringLiteral(".exe")))
		path += QStringLiteral(".exe");
#elif defined(Q_OS_OSX)
	if(path.endsWith(QStringLiteral(".app")))
		path.truncate(path.lastIndexOf(QStringLiteral(".")));
	path += QStringLiteral(".app/Contents/MacOS/") + QFileInfo{path}.fileName();
#endif

	QFileInfo mtInfo{QCoreApplication::applicationDirPath(), path};
	if (mtInfo.exists())
		return mtInfo;
	else
		return std::nullopt;
}

std::optional<QList<UpdateInfo>> QtIfwUpdaterBackend::parseUpdates()
{
	try {
		const auto outString = QString::fromUtf8(_process->readAllStandardOutput());
		const auto xmlBegin = outString.indexOf(QStringLiteral("<updates>"));
		if(xmlBegin < 0)
			return QList<UpdateInfo>{};
		const auto xmlEnd = outString.indexOf(QStringLiteral("</updates>"), xmlBegin);
		if(xmlEnd < 0)
			return QList<UpdateInfo>{};

		QList<UpdateInfo> updates;
		QXmlStreamReader reader(outString.mid(xmlBegin, (xmlEnd + 10) - xmlBegin));

		if (!reader.readNextStartElement()) {
			checkReader(reader);
			Q_UNREACHABLE();
		} else
			Q_ASSERT(reader.name() == QStringLiteral("updates"));  // assert here because string limitation already ensures this

		while(reader.readNextStartElement()) {
			if(reader.name() != QStringLiteral("update"))
				throwUnexpectedElement(reader);

			auto ok = false;
			UpdateInfo info;
			info.setName(reader.attributes().value(QStringLiteral("name")).toString());
			info.setVersion(QVersionNumber::fromString(reader.attributes().value(QStringLiteral("version")).toString()));
			info.setSize(reader.attributes().value(QStringLiteral("size")).toULongLong(&ok));

			if(info.name().isEmpty() || info.version().isNull() || !ok) {
				qCCritical(logCat()) << "Invalid <update> XML-Element, attributes are incomplete or unparsable";
				throw std::nullopt;
			} if(reader.readNextStartElement())
				throwUnexpectedElement(reader);
			else
				checkReader(reader);

			updates.append(info);
		}

		checkReader(reader);
		return updates;
	} catch (...) {
		return std::nullopt;
	}
}

void QtIfwUpdaterBackend::checkReader(QXmlStreamReader &reader)
{
	if(reader.hasError()) {
		qCCritical(logCat()) << "XML parse error:" << qUtf8Printable(reader.errorString());
		throw std::nullopt;
	}
}

void QtIfwUpdaterBackend::throwUnexpectedElement(QXmlStreamReader &reader)
{
	qCCritical(logCat()) << "Unexpected XML-Element" << reader.name();
	throw std::nullopt;
}
