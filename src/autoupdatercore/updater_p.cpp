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
	q_ptr(q_ptr),
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
	connect(this->scheduler, &SimpleScheduler::scheduleTriggered,
			this, &UpdaterPrivate::startUpdateCheck);
}

UpdaterPrivate::~UpdaterPrivate()
{
	if(this->runOnExit)
		qCWarning(logQtAutoUpdater, "Updater destroyed with run on exit active before the application quit");

	if(this->mainProcess &&
	   this->mainProcess->state() != QProcess::NotRunning) {
		this->mainProcess->kill();
		this->mainProcess->waitForFinished(1000);
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
	if(this->running)
		return false;
	else {
		Q_Q(Updater);
		this->updateInfos.clear();
		this->normalExit = true;
		this->lastErrorCode = EXIT_SUCCESS;
		this->lastErrorLog.clear();

		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), this->toolPath);
		this->mainProcess = new QProcess(this);
		this->mainProcess->setProgram(toolInfo.absoluteFilePath());
		this->mainProcess->setArguments({QStringLiteral("--checkupdates")});

		connect(this->mainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
				this, &UpdaterPrivate::updaterReady, Qt::QueuedConnection);
		connect(this->mainProcess, &QProcess::errorOccurred,
				this, &UpdaterPrivate::updaterError, Qt::QueuedConnection);

		this->mainProcess->start(QIODevice::ReadOnly);
		this->running = true;

		emit q->updateInfoChanged(this->updateInfos);
		emit q->runningChanged(true);
		return true;
	}
}

void UpdaterPrivate::stopUpdateCheck(int delay, bool async)
{
	if(this->mainProcess &&
	   this->mainProcess->state() != QProcess::NotRunning) {
		if(delay > 0) {
			this->mainProcess->terminate();
			if(async){
				QTimer::singleShot(delay, this, [this](){
					this->stopUpdateCheck(0, false);
				});
			} else {
				if(!this->mainProcess->waitForFinished(delay))
					this->mainProcess->kill();
			}
		} else
			this->mainProcess->kill();
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
	if(this->mainProcess) {
		if(exitStatus == QProcess::NormalExit) {
			this->normalExit = true;
			this->lastErrorCode = exitCode;
			this->lastErrorLog = this->mainProcess->readAllStandardError();
			const auto updateOut = this->mainProcess->readAllStandardOutput();
			this->mainProcess->deleteLater();
			this->mainProcess = nullptr;

			Q_Q(Updater);
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

void UpdaterPrivate::updaterError(QProcess::ProcessError error)
{
	if(this->mainProcess) {
		Q_Q(Updater);
		this->normalExit = false;
		this->lastErrorCode = error;
		this->lastErrorLog = this->mainProcess->errorString().toUtf8();
		this->mainProcess->deleteLater();
		this->mainProcess = nullptr;

		this->running = false;
		emit q->runningChanged(false);
		emit q->checkUpdatesDone(false, true);
	}
}

void UpdaterPrivate::appAboutToExit()
{
	if(this->runOnExit) {
		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), this->toolPath);
		auto ok = false;
		if(this->adminAuth && !this->adminAuth->hasAdminRights()) {
			ok = this->adminAuth->executeAsAdmin(toolInfo.absoluteFilePath(),
												 this->runArguments);

		} else {
			ok = QProcess::startDetached(toolInfo.absoluteFilePath(),
										 this->runArguments,
										 toolInfo.absolutePath());
		}

		if(!ok) {
			qCWarning(logQtAutoUpdater) << "Unable to start" << toolInfo.absoluteFilePath()
										<< "with arguments" << this->runArguments
										<< "as" << (this->adminAuth ? "admin/root" : "current user");
		}

		this->runOnExit = false;//prevent warning
	}
}
