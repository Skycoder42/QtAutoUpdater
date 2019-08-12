#include "qtifwupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>
using namespace QtAutoUpdater;

QtIfwUpdaterBackend::QtIfwUpdaterBackend(QObject *parent) :
	UpdaterBackend{parent}
{}

UpdaterBackend::Features QtIfwUpdaterBackend::features() const
{
	return Feature::CheckUpdates |
#ifdef Q_OS_WIN
			Feature::InstallNeedsExit |
#endif
			Feature::TriggerInstall;
}

bool QtIfwUpdaterBackend::initialize(const QVariantMap &arguments, AdminAuthoriser *authoriser)
{
	_authoriser = authoriser;

	auto mtInfo = findMaintenanceTool(arguments);
	if (!mtInfo)
		return false;

	_process = new QProcess{this};
	_process->setProgram(mtInfo->absoluteFilePath());
	_process->setWorkingDirectory(mtInfo->absolutePath());
	_process->setArguments({QStringLiteral("--checkupdates")});

	connect(_process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
			this, &QtIfwUpdaterBackend::updaterReady);
	connect(_process, &QProcess::errorOccurred,
			this, &QtIfwUpdaterBackend::updaterError);

	_silentUpdate = arguments[QStringLiteral("silent")].toBool();

	return true;
}

UpdateInstaller *QtIfwUpdaterBackend::installUpdates(const QList<UpdateInfo> &)
{
	return nullptr;
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

bool QtIfwUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &)
{
	QStringList arguments { _silentUpdate ?
							QStringLiteral("--silentUpdate") :
							QStringLiteral("--updater")};

	if (true && !_authoriser->hasAdminRights())
		return _authoriser->executeAsAdmin(_process->program(), arguments);
	else
		return QProcess::startDetached(_process->program(), arguments, _process->workingDirectory());
}

void QtIfwUpdaterBackend::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitStatus == QProcess::NormalExit) {
		if (exitCode == EXIT_SUCCESS) {
			auto updates = parseUpdates();
			if (updates)
				emit checkDone({});
			else
				emit error(tr("Read invalid output from MaintenanceTool"));
		} else
			emit checkDone({});
	}
	_process->close();
}

void QtIfwUpdaterBackend::updaterError()
{
	emit error(_process->errorString());
}

std::optional<QFileInfo> QtIfwUpdaterBackend::findMaintenanceTool(const QVariantMap &arguments)
{
	QString path;
	if (arguments.contains(QStringLiteral("path")))
		path = arguments[QStringLiteral("path")].toString();
	else
#ifdef Q_OS_OSX
		path = QStringLiteral("../../maintenancetool");
#else
		path =  QStringLiteral("./maintenancetool");
#endif

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
	const auto outString = QString::fromUtf8(_process->readAllStandardOutput());
	const auto xmlBegin = outString.indexOf(QStringLiteral("<updates>"));
	if(xmlBegin < 0)
		return QList<UpdateInfo>{};
	const auto xmlEnd = outString.indexOf(QStringLiteral("</updates>"), xmlBegin);
	if(xmlEnd < 0)
		return QList<UpdateInfo>{};

	QList<UpdateInfo> updates;
	QXmlStreamReader reader(outString.mid(xmlBegin, (xmlEnd + 10) - xmlBegin));

	reader.readNextStartElement();
	Q_ASSERT(reader.name() == QStringLiteral("updates"));

	while(reader.readNextStartElement()) {
		if(reader.name() != QStringLiteral("update"))
			return std::nullopt;

		auto ok = false;
		UpdateInfo info;
		info.setName(reader.attributes().value(QStringLiteral("name")).toString());
		info.setVersion(QVersionNumber::fromString(reader.attributes().value(QStringLiteral("version")).toString()));
		info.setSize(reader.attributes().value(QStringLiteral("size")).toULongLong(&ok));

		if(info.name().isEmpty() || info.version().isNull() || !ok)
			return std::nullopt;
		if(reader.readNextStartElement())
			return std::nullopt;

		updates.append(info);
	}

	if(reader.hasError()) {
		qWarning() << "XML-reader-error:" << reader.errorString();
		return std::nullopt;
	}

	return updates;
}
