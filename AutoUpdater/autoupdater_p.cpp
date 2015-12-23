#include "autoupdater.h"
#include "autoupdater_p.h"
#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#include <QTimerEvent>
#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

AutoUpdaterPrivate::AutoUpdaterPrivate(AutoUpdater *q_ptr) :
	QObject(NULL),
	q_ptr(q_ptr),
	toolPath(AutoUpdaterPrivate::toSystemExe(QStringLiteral("./maintenancetool"))),
	updateInfos(),
	normalExit(true),
	lastErrorCode(EXIT_SUCCESS),
	lastErrorLog(),
	running(false),
	mainProcess(NULL),
	activeTimers(),
	runOnExit(false),
	runArguments(),
	runAdmin(false)
{
	connect(qApp, &QCoreApplication::aboutToQuit,
			this, &AutoUpdaterPrivate::appAboutToExit);
}

AutoUpdaterPrivate::~AutoUpdaterPrivate()
{
	if(this->mainProcess &&
	   this->mainProcess->state() != QProcess::NotRunning) {
		this->mainProcess->kill();
	}
}

const QString AutoUpdaterPrivate::toSystemExe(const QString basePath)
{
#if defined(Q_OS_WIN)
	return basePath + QStringLiteral(".exe");
#elif defined(Q_OS_MAC)//TODO check
	QFileInfo info(basePath);
	return basePath + QStringLiteral(".app/Contents/MacOS/") + info.fileName();
#elif defined(Q_OS_UNIX)//TODO check (x11?)
	return basePath;
#endif
}

bool AutoUpdaterPrivate::startUpdateCheck()
{
	if(this->running)
		return false;
	else {
		Q_Q(AutoUpdater);
		this->updateInfos.clear();
		this->normalExit = true;
		this->lastErrorCode = EXIT_SUCCESS;
		this->lastErrorLog.clear();

		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), this->toolPath);
		this->mainProcess = new QProcess(this);
		this->mainProcess->setProgram(toolInfo.absoluteFilePath());
		this->mainProcess->setArguments({QStringLiteral("--checkupdates")});
		this->mainProcess->setWorkingDirectory(toolInfo.absolutePath());//TODO mac...

		connect(this->mainProcess, SELECT<int>::OVERLOAD_OF(&QProcess::finished),
				this, &AutoUpdaterPrivate::updaterReady, Qt::QueuedConnection);
		connect(this->mainProcess, SELECT<QProcess::ProcessError>::OVERLOAD_OF(&QProcess::error),
				this, &AutoUpdaterPrivate::updaterError, Qt::QueuedConnection);

		this->mainProcess->start(QIODevice::ReadOnly);
		this->running = true;

		emit q->updateInfoChanged(this->updateInfos);
		emit q->runningChanged(true);
		return true;
	}
}

void AutoUpdaterPrivate::stopUpdateCheck(int delay)
{
	if(this->mainProcess &&
	   this->mainProcess->state() != QProcess::NotRunning) {
		if(delay > 0) {
			this->mainProcess->terminate();
			if(this->mainProcess->waitForFinished(delay))
				return;
		}

		this->mainProcess->kill();
		//TODO test if error/finished are still emitted
	}
}

void AutoUpdaterPrivate::updaterReady(int exitCode)
{
	if(this->mainProcess) {
		if(this->mainProcess->exitStatus() == QProcess::NormalExit) {
			this->normalExit = true;
			this->lastErrorCode = exitCode;
			this->lastErrorLog = this->mainProcess->readAllStandardError();
			QByteArray updateOut = this->mainProcess->readAllStandardOutput();
			this->mainProcess->deleteLater();
			this->mainProcess = NULL;

			Q_Q(AutoUpdater);
			if(this->lastErrorCode != EXIT_SUCCESS) {
				this->running = false;
				emit q->runningChanged(false);
				emit q->checkUpdatesDone(false, true);
			} else {
				this->running = false;
				emit q->runningChanged(false);
				try {
					this->updateInfos = this->parseResult(updateOut);
					if(!this->updateInfos.isEmpty())
						emit q->updateInfoChanged(this->updateInfos);
					emit q->checkUpdatesDone(!this->updateInfos.isEmpty(), false);
				} catch (NoUpdatesXmlException &) {
					emit q->checkUpdatesDone(false, false);
				} catch (UpdateParseException &exc) {
					this->lastErrorLog = exc.what();
					emit q->checkUpdatesDone(false, true);
				}
			}
		} else
			this->updaterError(QProcess::Crashed);
	}
}

void AutoUpdaterPrivate::updaterError(QProcess::ProcessError error)
{
	if(this->mainProcess) {
		Q_Q(AutoUpdater);
		this->normalExit = false;
		this->lastErrorCode = error;
		this->lastErrorLog = this->mainProcess->errorString().toUtf8();
		this->mainProcess->deleteLater();
		this->mainProcess = NULL;

		this->running = false;
		emit q->runningChanged(false);
		emit q->checkUpdatesDone(false, true);
	}
}

QList<AutoUpdater::UpdateInfo> AutoUpdaterPrivate::parseResult(const QByteArray &output)
{
	QString outString = QString::fromUtf8(output);
	int xmlBegin = outString.indexOf(QStringLiteral("<updates>"));
	if(xmlBegin < 0)
		throw NoUpdatesXmlException();
	int xmlEnd = outString.indexOf(QStringLiteral("</updates>"), xmlBegin);
	if(xmlEnd < 0)
		throw NoUpdatesXmlException();

	QList<AutoUpdater::UpdateInfo> updates;
	QXmlStreamReader reader(outString.mid(xmlBegin, xmlEnd - xmlBegin + 10));

	reader.readNextStartElement();
	//should always work because it was search for
	Q_ASSERT(reader.name() == QStringLiteral("updates"));

	while(reader.readNextStartElement()) {
		if(reader.name() != QStringLiteral("update"))
			throw InvalidXmlException();

		bool ok = false;
		AutoUpdater::UpdateInfo info;
		info.name = reader.attributes().value(QStringLiteral("name")).toString();
		info.version = QVersionNumber::fromString(reader.attributes().value(QStringLiteral("version")).toString());
		info.size = reader.attributes().value(QStringLiteral("size")).toULongLong(&ok);

		if(info.name.isEmpty() || info.version.isNull() || !ok)
			throw InvalidXmlException();
		if(reader.readNextStartElement())
			throw InvalidXmlException();

		updates.append(info);
	}

	if(reader.hasError()) {
		qWarning() << "XML-reader-error:" << reader.errorString();
		throw InvalidXmlException();
	}

	return updates;
}

void AutoUpdaterPrivate::appAboutToExit()
{
	if(this->runOnExit) {
		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), this->toolPath);
		if(!QProcess::startDetached(toolInfo.absoluteFilePath(),
									this->runArguments,
									toolInfo.absolutePath()))//TODO mac...
			qWarning() << "Unable to start" << toolInfo.absolutePath()
					   << "with arguments" << this->runArguments
					   << "as" << (this->runAdmin ? "admin" : "user");
	}
}

void AutoUpdaterPrivate::timerEvent(QTimerEvent *event)
{
	if(!this->activeTimers[event->timerId()]) {
		this->killTimer(event->timerId());
		this->activeTimers.remove(event->timerId());
	}
	event->accept();
	this->startUpdateCheck();
}
