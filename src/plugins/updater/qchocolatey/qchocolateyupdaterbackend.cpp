#include "qchocolateyupdaterbackend.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logChocoBackend, "qt.autoupdater.core.plugin.chocolatey.backend")

const QString QChocolateyUpdaterBackend::KeyPackages {QStringLiteral("packages")};
const QString QChocolateyUpdaterBackend::KeyPath {QStringLiteral("path")};
const QString QChocolateyUpdaterBackend::KeyExtraCheckArgs {QStringLiteral("extraCheckArgs")};
const QString QChocolateyUpdaterBackend::KeyGuiExePath {QStringLiteral("guiExePath")};
const QString QChocolateyUpdaterBackend::KeyExtraGuiArgs {QStringLiteral("extraGuiArgs")};
const QString QChocolateyUpdaterBackend::KeyRunAsAdmin {QStringLiteral("runAsAdmin")};

const QString QChocolateyUpdaterBackend::DefaultGuiExePath {QStringLiteral(R"_(C:\Program Files (x86)\Chocolatey GUI\ChocolateyGui.exe)_")};

QChocolateyUpdaterBackend::QChocolateyUpdaterBackend(QString &&key, QObject *parent) :
	ProcessBackend{std::move(key), parent}
{}

UpdaterBackend::Features QChocolateyUpdaterBackend::features() const
{
	return QFileInfo{guiPath()}.isExecutable() ?
				Feature::TriggerInstall :
	Feature::CheckUpdates;
}

UpdaterBackend::SecondaryInfo QChocolateyUpdaterBackend::secondaryInfo() const
{
	return std::make_pair(QStringLiteral("oldVersion"), tr("Installed Version"));
}

void QChocolateyUpdaterBackend::checkForUpdates()
{
	UpdateProcessInfo info;

	info.program = chocoPath();
	if (info.program.isEmpty()) {
		emit checkDone(false);
		return;
	}

	info.arguments = QStringList {
		QStringLiteral("outdated"),
		QStringLiteral("--no-color"),
		QStringLiteral("--confirm"),
		QStringLiteral("--no-progress"),
		QStringLiteral("--ignore-unfound")
	};
	if (auto extraArgs = config()->value(KeyExtraCheckArgs); extraArgs)
		info.arguments += readArgumentList(*extraArgs);

	runUpdateTool(0, std::move(info));
}

UpdateInstaller *QChocolateyUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QChocolateyUpdaterBackend::initialize()
{
	if (auto pConf = config()->value(KeyPackages); pConf)
		_packages = readStringList(*pConf);
	if (_packages.isEmpty()) {
		qCCritical(logChocoBackend) << "Configuration for chocolatey must contain 'packages' with at least one package";
		return false;
	}

	return !chocoPath().isEmpty();
}

void QChocolateyUpdaterBackend::onToolDone(int id, int exitCode, QIODevice *processDevice)
{
	Q_ASSERT(id == 0);
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
			info.setIdentifier(info.name());
			if (!_packages.contains(info.name()))
				continue;
			info.setVersion(QVersionNumber::fromString(QString::fromUtf8(line[2])));
			info.setData(QStringLiteral("oldVersion"), QVariant::fromValue(QVersionNumber::fromString(QString::fromUtf8(line[1]))));
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

	if (auto extraArgs = config()->value(KeyExtraGuiArgs); extraArgs)
		info.arguments += readArgumentList(*extraArgs);

	info.runAsAdmin = config()->value(KeyRunAsAdmin, true).toBool();

	return info;
}

QString QChocolateyUpdaterBackend::chocoPath() const
{
	QStringList paths;
	if (auto mPaths = config()->value(KeyPath); mPaths)
		paths = readPathList(*mPaths);

	const auto path = QStandardPaths::findExecutable(QStringLiteral("choco"), paths);
	if (path.isEmpty()) {
		qCCritical(logChocoBackend) << "Failed to find choco executable";
		return {};
	} else
		return path;
}

QString QChocolateyUpdaterBackend::guiPath() const
{
	return config()->value(KeyGuiExePath, DefaultGuiExePath).toString();
}
