#include "qchocolateyupdaterbackend.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logChocoBackend, "qt.autoupdater.core.plugin.chocolatey.backend")

const QString QChocolateyUpdaterBackend::DefaultGuiPath {QStringLiteral(R"_(C:\Program Files (x86)\Chocolatey GUI\ChocolateyGui.exe)_")};

QChocolateyUpdaterBackend::QChocolateyUpdaterBackend(QString &&key, QObject *parent) :
	ProcessBackend{std::move(key), parent}
{}

UpdaterBackend::Features QChocolateyUpdaterBackend::features() const
{
	return QFileInfo{guiPath()}.isExecutable() ?
				Feature::TriggerInstall :
				Feature::CheckUpdates;
}

UpdateInstaller *QChocolateyUpdaterBackend::createInstaller()
{
	return nullptr;
}

std::optional<ProcessBackend::UpdateProcessInfo> QChocolateyUpdaterBackend::initializeImpl()
{
	if (auto pConf = config()->value(QStringLiteral("packages")); pConf) {
		if (pConf->userType() == QMetaType::QStringList)
			_packages = pConf->toStringList();
		else
			_packages = pConf->toString().split(QLatin1Char(','));
	}
	if (_packages.isEmpty()) {
		qCCritical(logChocoBackend) << "Configuration for chocolatey must contain 'packages' with at least one package";
		return std::nullopt;
	}

	QStringList paths;
	if (auto mPaths = config()->value(QStringLiteral("path")); mPaths) {
		if (mPaths->userType() == QMetaType::QStringList)
			paths = mPaths->toStringList();
		else
			paths = mPaths->toString().split(QDir::listSeparator());
	}

	UpdateProcessInfo info;
	info.program = QStandardPaths::findExecutable(QStringLiteral("choco"), paths);
	if (info.program.isEmpty()) {
		qCCritical(logChocoBackend) << "Failed to find choco executable";
		return std::nullopt;
	}

	info.arguments = QStringList {
		QStringLiteral("outdated"),
		QStringLiteral("--no-color"),
		QStringLiteral("--confirm"),
		QStringLiteral("--no-progress"),
		QStringLiteral("--ignore-unfound")
	};
	if (auto argsVal = config()->value(QStringLiteral("extraCheckArgs")); argsVal) {
		if (argsVal->userType() == QMetaType::QStringList)
			info.arguments.append(argsVal->toStringList());
		else
			info.arguments.append(argsVal->toString().split(QLatin1Char(' ')));
	}

	return info;
}

void QChocolateyUpdaterBackend::parseResult(int exitCode, QIODevice *processDevice)
{
	switch (exitCode) {
	case 0:
	case 2: {
		QList<UpdateInfo> updates;
		while (!processDevice->atEnd()) {
			const auto line = processDevice->readLine().trimmed().split('|');
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
		break;
	}
	default:
		qCCritical(logChocoBackend) << "choco finished with an unclean exit code:" << exitCode;
		emit checkDone(false);
		break;
	}
}

std::optional<ProcessBackend::InstallProcessInfo> QChocolateyUpdaterBackend::installerInfo(const QList<UpdateInfo> &infos, bool track)
{
	Q_UNUSED(infos)
	Q_UNUSED(track)

	InstallProcessInfo info;
	info.program = guiPath();
	if (!QFileInfo{info.program}.isExecutable()) {
		qCCritical(logChocoBackend) << "Failed to find Chocolatey GUI executable";
		return std::nullopt;
	}

	if (auto extraArgs = config()->value(QStringLiteral("extraInstallArgs")); extraArgs) {
		if (extraArgs->userType() == QMetaType::QStringList)
			info.arguments.append(extraArgs->toStringList());
		else
			info.arguments.append(extraArgs->toString().split(QLatin1Char(' ')));
	}

	info.runAsAdmin = config()->value(QStringLiteral("runAsAdmin"), true).toBool();

	return info;
}

QString QChocolateyUpdaterBackend::guiPath() const
{
	return config()->value(QStringLiteral("guiExePath"), DefaultGuiPath).toString();
}
