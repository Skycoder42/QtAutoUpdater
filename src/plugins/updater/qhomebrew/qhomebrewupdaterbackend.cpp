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

void QHomebrewUpdaterBackend::checkForUpdates()
{
	UpdateProcessInfo info;
	info.program = brewPath();
	if (info.program.isEmpty()) {
		emit checkDone(false);
		return;
	}

	info.arguments = QStringList {
		QStringLiteral("update")
	};
	if (auto argsVal = config()->value(QStringLiteral("extraUpdateArgs")); argsVal) {
		if (argsVal->userType() == QMetaType::QStringList)
			info.arguments.append(argsVal->toStringList());
		else
			info.arguments.append(argsVal->toString().split(QLatin1Char(' ')));
	}

	info.useStdout = false;
	runUpdateTool(Update, std::move(info));
}

UpdateInstaller *QHomebrewUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QHomebrewUpdaterBackend::initialize()
{
	if (auto pConf = config()->value(QStringLiteral("packages")); pConf) {
		if (pConf->userType() == QMetaType::QStringList)
			_packages = pConf->toStringList();
		else
			_packages = pConf->toString().split(QLatin1Char(','));
	}
	if (_packages.isEmpty()) {
		qCCritical(logBrewBackend) << "Configuration for chocolatey must contain 'packages' with at least one package";
		return false;
	}

	return !brewPath().isEmpty();
}

void QHomebrewUpdaterBackend::onToolDone(int id, int exitCode, QIODevice *processDevice)
{
	switch (id) {
	case Update:
		onUpdated(exitCode);
		break;
	case Outdated:
		onOutdated(exitCode, processDevice);
		break;
	default:
		Q_UNREACHABLE();
		break;
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

QString QHomebrewUpdaterBackend::brewPath() const
{
	QStringList paths;
	if (auto mPaths = config()->value(QStringLiteral("path")); mPaths) {
		if (mPaths->userType() == QMetaType::QStringList)
			paths = mPaths->toStringList();
		else
			paths = mPaths->toString().split(QDir::listSeparator());
	}

	const auto path = QStandardPaths::findExecutable(QStringLiteral("brew"), paths);
	if (path.isEmpty()) {
		qCCritical(logBrewBackend) << "Failed to find brew executable";
		return {};
	} else
		return path;
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

void QHomebrewUpdaterBackend::onUpdated(int exitCode)
{
	if (exitCode == EXIT_SUCCESS) {
		UpdateProcessInfo info;
		info.program = brewPath();
		Q_ASSERT(!info.program.isEmpty());
		info.arguments = QStringList {
			QStringLiteral("outdated"),
			QStringLiteral("--json=v1")
		};
		if (auto argsVal = config()->value(QStringLiteral("extraOutdatedArgs")); argsVal) {
			if (argsVal->userType() == QMetaType::QStringList)
				info.arguments.append(argsVal->toStringList());
			else
				info.arguments.append(argsVal->toString().split(QLatin1Char(' ')));
		}
		runUpdateTool(Outdated, std::move(info));
	} else {
		qCCritical(logBrewBackend) << "brew update exited with error code" << exitCode;
		emit checkDone(false);
	}
}

void QHomebrewUpdaterBackend::onOutdated(int exitCode, QIODevice *processDevice)
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
		qCCritical(logBrewBackend) << "brew outdated exited with error code" << exitCode;
		emit checkDone(false);
	}
}
