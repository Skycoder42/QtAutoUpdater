#include "autoupdater.h"
#include "autoupdater_p.h"
#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

AutoUpdaterPrivate::AutoUpdaterPrivate(AutoUpdater *q_ptr) :
	q_ptr(q_ptr),
	toolPath(),
	updateInfos(),
	normalExit(true),
	lastErrorCode(EXIT_SUCCESS),
	lastErrorLog(),
	running(false),
	workingToolPath(),
	mainProcess(NULL)
{
	//TODO test if ok on mac...
	this->toolPath = AutoUpdaterPrivate::toSystemExe(QStringLiteral("./maintenancetool"));
//	this->mainInfo.runArgs = {QStringLiteral("--updater")};
	//	this->mainInfo.runAdmin = false;
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
		this->workingToolPath = this->toolPath;
		this->lastErrorCode = EXIT_SUCCESS;
		this->lastErrorLog.clear();

		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), this->workingToolPath);
		this->mainProcess = new QProcess(q);
		this->mainProcess->setProgram(toolInfo.absoluteFilePath());
		this->mainProcess->setArguments({QStringLiteral("--checkupdates")});
		this->mainProcess->setWorkingDirectory(toolInfo.absolutePath());//TODO mac...

		QObject::connect(this->mainProcess, SELECT<int>::OVERLOAD_OF(&QProcess::finished),
				q, [this](int exitCode){//QProcess::ExitStatus is not registered as metatype...
			this->updaterReady(exitCode, this->mainProcess->exitStatus());
		}, Qt::QueuedConnection);
		QObject::connect(this->mainProcess, SELECT<QProcess::ProcessError>::OVERLOAD_OF(&QProcess::error),
				q, [this](const QProcess::ProcessError &error){
			this->updaterError(error);
		}, Qt::QueuedConnection);

		this->mainProcess->start(QIODevice::ReadOnly);
		this->running = true;
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

void AutoUpdaterPrivate::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(this->mainProcess) {
		if(exitStatus == QProcess::NormalExit) {
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
	QXmlStreamReader reader(outString.mid(xmlBegin, xmlEnd - xmlBegin + 10).simplified());

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
