#include "updater.h"
#include "updater_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QTimer>

using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logQtAutoUpdater, "QtAutoUpdater")

UpdaterPrivate::UpdaterPrivate(Updater *q_ptr) :
	QObject(nullptr),
	q(q_ptr),
	toolPath(),
	updateInfos(),
	normalExit(true),
	lastErrorCode(EXIT_SUCCESS),
	lastErrorLog(),
	running(false),
	mainProcess(nullptr),
	scheduler(new SimpleScheduler(this)),
	runOnExit(false),
	runArguments(),
	adminAuth(nullptr)
{
	connect(qApp, &QCoreApplication::aboutToQuit,
			this, &UpdaterPrivate::appAboutToExit,
			Qt::DirectConnection);
	connect(scheduler, &SimpleScheduler::scheduleTriggered,
			this, &UpdaterPrivate::startUpdateCheck);
}

UpdaterPrivate::~UpdaterPrivate()
{
	if(runOnExit)
		qCWarning(logQtAutoUpdater) << "Updater destroyed with run on exit active before the application quit";

	if(mainProcess &&
	   mainProcess->state() != QProcess::NotRunning) {
		mainProcess->kill();
		mainProcess->waitForFinished(1000);
	}
}

const QString UpdaterPrivate::toSystemExe(QString basePath)
{
#if defined(Q_OS_WIN32)
	if(!basePath.endsWith(QStringLiteral(".exe")))
		return basePath + QStringLiteral(".exe");
	else
		return basePath;
#elif defined(Q_OS_OSX)
	if(basePath.endsWith(QStringLiteral(".app")))
		basePath.truncate(basePath.lastIndexOf(QStringLiteral(".")));
	return basePath + QStringLiteral(".app/Contents/MacOS/") + QFileInfo(basePath).fileName();
#elif defined(Q_OS_UNIX)
	return basePath;
#endif
}

bool UpdaterPrivate::startUpdateCheck()
{
	if(running)
		return false;
	else {
		updateInfos.clear();
		normalExit = true;
		lastErrorCode = EXIT_SUCCESS;
		lastErrorLog.clear();

		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), toolPath);
		mainProcess = new QProcess(this);
		mainProcess->setProgram(toolInfo.absoluteFilePath());
		mainProcess->setArguments({QStringLiteral("--checkupdates")});

		connect(mainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
				this, &UpdaterPrivate::updaterReady, Qt::QueuedConnection);
		connect(mainProcess, &QProcess::errorOccurred,
				this, &UpdaterPrivate::updaterError, Qt::QueuedConnection);

		mainProcess->start(QIODevice::ReadOnly);
		running = true;

		emit q->updateInfoChanged(updateInfos);
		emit q->runningChanged(true);
		return true;
	}
}

void UpdaterPrivate::stopUpdateCheck(int delay, bool async)
{
	if(mainProcess &&
	   mainProcess->state() != QProcess::NotRunning) {
		if(delay > 0) {
			mainProcess->terminate();
			if(async){
				QTimer::singleShot(delay, this, [this](){
					stopUpdateCheck(0, false);
				});
			} else {
				if(!mainProcess->waitForFinished(delay)) {
					mainProcess->kill();
					mainProcess->waitForFinished(100);
				}
			}
		} else {
			mainProcess->kill();
			mainProcess->waitForFinished(100);
		}
	}
}

QList<Updater::UpdateInfo> UpdaterPrivate::parseResult(const QByteArray &output)
{
	const auto outString = QString::fromUtf8(output);
	const auto xmlBegin = outString.indexOf(QStringLiteral("<updates>"));
	if(xmlBegin < 0)
		throw NoUpdatesXmlException();
	const auto xmlEnd = outString.indexOf(QStringLiteral("</updates>"), xmlBegin);
	if(xmlEnd < 0)
		throw NoUpdatesXmlException();

	QList<Updater::UpdateInfo> updates;
	QXmlStreamReader reader(outString.mid(xmlBegin, (xmlEnd + 10) - xmlBegin));

	reader.readNextStartElement();
	//should always work because it was search for
	Q_ASSERT(reader.name() == QStringLiteral("updates"));

	while(reader.readNextStartElement()) {
		if(reader.name() != QStringLiteral("update"))
			throw InvalidXmlException();

		auto ok = false;
		Updater::UpdateInfo info(reader.attributes().value(QStringLiteral("name")).toString(),
								 QVersionNumber::fromString(reader.attributes().value(QStringLiteral("version")).toString()),
								 reader.attributes().value(QStringLiteral("size")).toULongLong(&ok));

		if(info.name.isEmpty() || info.version.isNull() || !ok)
			throw InvalidXmlException();
		if(reader.readNextStartElement())
			throw InvalidXmlException();

		updates.append(info);
	}

	if(reader.hasError()) {
		qCWarning(logQtAutoUpdater) << "XML-reader-error:" << reader.errorString();
		throw InvalidXmlException();
	}

	return updates;
}

void UpdaterPrivate::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(mainProcess) {
		if(exitStatus == QProcess::NormalExit) {
			normalExit = true;
			lastErrorCode = exitCode;
			lastErrorLog = mainProcess->readAllStandardError();
			const auto updateOut = mainProcess->readAllStandardOutput();
			mainProcess->deleteLater();
			mainProcess = nullptr;

			running = false;
			emit q->runningChanged(false);
			try {
				updateInfos = parseResult(updateOut);
				if(!updateInfos.isEmpty())
					emit q->updateInfoChanged(updateInfos);
				emit q->checkUpdatesDone(!updateInfos.isEmpty(), false);
			} catch (NoUpdatesXmlException &) {
				emit q->checkUpdatesDone(false, false);
			} catch (InvalidXmlException &exc) {
				lastErrorLog = exc.what();
				emit q->checkUpdatesDone(false, true);
			}
		} else
			updaterError(QProcess::Crashed);
	}
}

void UpdaterPrivate::updaterError(QProcess::ProcessError error)
{
	if(mainProcess) {
		normalExit = false;
		lastErrorCode = error;
		lastErrorLog = mainProcess->errorString().toUtf8();
		mainProcess->deleteLater();
		mainProcess = nullptr;

		running = false;
		emit q->runningChanged(false);
		emit q->checkUpdatesDone(false, true);
	}
}

void UpdaterPrivate::appAboutToExit()
{
	if(runOnExit) {
		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), toolPath);
		auto ok = false;
		if(adminAuth && !adminAuth->hasAdminRights())
			ok = adminAuth->executeAsAdmin(toolInfo.absoluteFilePath(), runArguments);
		else {
			ok = QProcess::startDetached(toolInfo.absoluteFilePath(),
										 runArguments,
										 toolInfo.absolutePath());
		}

		if(!ok) {
			qCWarning(logQtAutoUpdater) << "Unable to start" << toolInfo.absoluteFilePath()
										<< "with arguments" << runArguments
										<< "as" << (adminAuth ? "admin/root" : "current user");
		}

		runOnExit = false;//prevent warning
	}
}



const char *UpdaterPrivate::NoUpdatesXmlException::what() const noexcept
{
	return "The <updates> node could not be found";
}

void UpdaterPrivate::NoUpdatesXmlException::raise() const
{
	throw *this;
}

QException *UpdaterPrivate::NoUpdatesXmlException::clone() const
{
	return new NoUpdatesXmlException();
}

const char *UpdaterPrivate::InvalidXmlException::what() const noexcept
{
	return "The found XML-part is not of a valid updates-XML-format";
}

void UpdaterPrivate::InvalidXmlException::raise() const
{
	throw *this;
}

QException *UpdaterPrivate::InvalidXmlException::clone() const
{
	return new InvalidXmlException();
}
