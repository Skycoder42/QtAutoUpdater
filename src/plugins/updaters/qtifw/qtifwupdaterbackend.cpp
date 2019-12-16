#include "qtifwupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logQtIfwBackend, "qt.autoupdater.core.plugin.qtifw.backend")

const QString QtIfwUpdaterBackend::KeyPath {QStringLiteral("path")};
const QString QtIfwUpdaterBackend::KeyExtraCheckArgs {QStringLiteral("extraCheckArgs")};
const QString QtIfwUpdaterBackend::KeySilent {QStringLiteral("silent")};
const QString QtIfwUpdaterBackend::KeyExtraInstallArgs {QStringLiteral("extraInstallArgs")};
const QString QtIfwUpdaterBackend::KeyRunAsAdmin {QStringLiteral("runAsAdmin")};

#ifdef Q_OS_OSX
const QString QtIfwUpdaterBackend::DefaultPath {QStringLiteral("../../maintenancetool")};
#else
const QString QtIfwUpdaterBackend::DefaultPath {QStringLiteral("./maintenancetool")};
#endif

QtIfwUpdaterBackend::QtIfwUpdaterBackend(QString &&key, QObject *parent) :
	ProcessBackend{std::move(key), parent}
{}

UpdaterBackend::Features QtIfwUpdaterBackend::features() const
{
	return Feature::CheckUpdates |
#ifndef Q_OS_WIN
			Feature::ParallelTrigger;
#else
			Feature::TriggerInstall;
#endif
}

UpdaterBackend::SecondaryInfo QtIfwUpdaterBackend::secondaryInfo() const
{
	return std::make_pair(QStringLiteral("size"), tr("Update-Size (Bytes)"));
}

void QtIfwUpdaterBackend::checkForUpdates()
{
	auto mtInfo = findMaintenanceTool();
	if (!mtInfo) {
		emit checkDone(false);
		return;
	}

	UpdateProcessInfo info;
	info.program = mtInfo->absoluteFilePath();
	info.workingDir = mtInfo->absolutePath();
	info.arguments = QStringList{QStringLiteral("--checkupdates")};
	if (auto extraArgs = config()->value(KeyExtraCheckArgs); extraArgs)
		info.arguments += readArgumentList(*extraArgs);
	runUpdateTool(0, std::move(info));
}

UpdateInstaller *QtIfwUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QtIfwUpdaterBackend::initialize()
{
	return static_cast<bool>(findMaintenanceTool());
}

void QtIfwUpdaterBackend::onToolDone(int id, int exitCode, QIODevice *processDevice)
{
	Q_ASSERT(id == 0);
	if (exitCode == EXIT_SUCCESS) {
		auto updates = parseUpdates(processDevice);
		if (updates)
			emit checkDone(true, *updates);
		else
			emit checkDone(false);
	} else
		emit checkDone(true);
}

std::optional<ProcessBackend::InstallProcessInfo> QtIfwUpdaterBackend::installerInfo(const QList<UpdateInfo> &infos, bool track)
{
	Q_UNUSED(infos)
	Q_UNUSED(track)

	auto mtInfo = findMaintenanceTool();
	if (!mtInfo)
		return std::nullopt;

	InstallProcessInfo info;
	info.program = mtInfo->absoluteFilePath();
	info.workingDir = mtInfo->absolutePath();
	info.arguments = QStringList{
		config()->value(KeySilent, DefaultSilent).toBool() ?
					QStringLiteral("--silentUpdate") :
					QStringLiteral("--updater")
	};
	if (auto extraArgs = config()->value(KeyExtraInstallArgs); extraArgs)
		info.arguments += readArgumentList(*extraArgs);
	if (auto runAsAdmin = config()->value(KeyRunAsAdmin); runAsAdmin)
		info.runAsAdmin = runAsAdmin->toBool();
	return info;
}

std::optional<QFileInfo> QtIfwUpdaterBackend::findMaintenanceTool()
{
	auto path = config()->value(KeyPath, DefaultPath).toString();

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
	else {
		qCCritical(logQtIfwBackend) << "Path to maintenancetool could not be determined or does not exist. "
									   "Use the 'path' configuration parameter to explicitly specify it";
		return std::nullopt;
	}
}

std::optional<QList<UpdateInfo>> QtIfwUpdaterBackend::parseUpdates(QIODevice *device)
{
	try {
		const auto outString = QString::fromUtf8(device->readAll());
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
			info.setIdentifier(info.name());
			info.setVersion(QVersionNumber::fromString(reader.attributes().value(QStringLiteral("version")).toString()));
			info.setData(QStringLiteral("size"), reader.attributes().value(QStringLiteral("size")).toULongLong(&ok));

			if(info.name().isEmpty() || info.version().isNull() || !ok) {
				qCCritical(logQtIfwBackend) << "Invalid <update> XML-Element, attributes are incomplete or unparsable";
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
		qCCritical(logQtIfwBackend) << "XML parse error:" << qUtf8Printable(reader.errorString());
		throw std::nullopt;
	}
}

void QtIfwUpdaterBackend::throwUnexpectedElement(QXmlStreamReader &reader)
{
	qCCritical(logQtIfwBackend) << "Unexpected XML-Element" << reader.name();
	throw std::nullopt;
}
