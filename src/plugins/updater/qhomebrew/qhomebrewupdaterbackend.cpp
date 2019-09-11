#include "qhomebrewupdaterbackend.h"
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logBrewBackend, "qt.autoupdater.core.plugin.homebrew.backend")

QHomebrewUpdaterBackend::QHomebrewUpdaterBackend(QString &&key, QObject *parent) :
	ProcessBackend{std::move(key), parent}
{}

UpdaterBackend::Features QHomebrewUpdaterBackend::features() const
{
	return QFileInfo{cakebrewPath()}.isExecutable() ?
				(Feature::TriggerInstall | Feature::ParallelInstall) :
				Feature::CheckUpdates;
}

UpdateInstaller *QHomebrewUpdaterBackend::createInstaller()
{
	return nullptr;
}

std::optional<ProcessBackend::UpdateProcessInfo> QHomebrewUpdaterBackend::initializeImpl()
{
	if (auto pConf = config()->value(QStringLiteral("packages")); pConf) {
		if (pConf->userType() == QMetaType::QStringList)
			_packages = pConf->toStringList();
		else
			_packages = pConf->toString().split(QLatin1Char(','));
	}
	if (_packages.isEmpty()) {
		qCCritical(logBrewBackend) << "Configuration for chocolatey must contain 'packages' with at least one package";
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
	info.program = QStandardPaths::findExecutable(QStringLiteral("brew"), paths);
	if (info.program.isEmpty()) {
		qCCritical(logBrewBackend) << "Failed to find brew executable";
		return std::nullopt;
	}

	// TODO run "update" instead and "outdated" in parse
	info.arguments = QStringList {
		QStringLiteral("outdated"),
		QStringLiteral("--json=v1")
	};
	if (auto argsVal = config()->value(QStringLiteral("extraCheckArgs")); argsVal) {
		if (argsVal->userType() == QMetaType::QStringList)
			info.arguments.append(argsVal->toStringList());
		else
			info.arguments.append(argsVal->toString().split(QLatin1Char(' ')));
	}

	return info;
}

void QHomebrewUpdaterBackend::parseResult(int exitCode, QIODevice *processDevice)
{
	if (exitCode == EXIT_SUCCESS) {
		QJsonParseError error;
		auto doc = QJsonDocument::fromJson(processDevice->readAll(), &error);
		if (error.error != QJsonParseError::NoError) {
			qCCritical(logBrewBackend) << "Failed to parse JSON-output at position"
									   << error.offset << "with error:"
									   << qUtf8Printable(error.errorString());
			emit checkDone(false);
			return;
		}
		if (!doc.isArray()) {
			qCCritical(logBrewBackend) << "JSON-output is not an array";
			emit checkDone(false);
			return;
		}

		QList<UpdateInfo> updates;
		for (const auto value : doc.array()) {
			const auto obj = value.toObject();
			UpdateInfo info;
			info.setName(obj[QStringLiteral("name")].toString());
			if (!_packages.contains(info.name()))
				continue;
			info.setVersion(QVersionNumber::fromString(obj[QStringLiteral("current_version")].toString()));
			info.setIdentifier(info.name());
			updates.append(info);
		}
		emit checkDone(true, updates);
	} else {
		qCCritical(logBrewBackend) << "brew exited with error code" << exitCode;
		emit checkDone(false);
	}
}

std::optional<ProcessBackend::InstallProcessInfo> QHomebrewUpdaterBackend::installerInfo(const QList<UpdateInfo> &infos, bool track)
{
	Q_UNUSED(infos)
	Q_UNUSED(track)

	InstallProcessInfo info;
	info.program = cakebrewPath();
	if (!QFileInfo{info.program}.isExecutable()) {
		qCCritical(logBrewBackend) << "Failed to find Cakebrew GUI app bundle";
		return std::nullopt;
	}

	if (auto extraArgs = config()->value(QStringLiteral("extraInstallArgs")); extraArgs) {
		if (extraArgs->userType() == QMetaType::QStringList)
			info.arguments.append(extraArgs->toStringList());
		else
			info.arguments.append(extraArgs->toString().split(QLatin1Char(' ')));
	}

	info.runAsAdmin = false;

	return info;
}

QString QHomebrewUpdaterBackend::cakebrewPath() const
{
	QDir cakeDir {QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
										 QStringLiteral("Cakebrew.app"),
										 QStandardPaths::LocateDirectory)};
	if (cakeDir.exists())
		return cakeDir.absoluteFilePath(QStringLiteral("Contents/MacOS/Cakebrew"));
	else
		return {};
}
