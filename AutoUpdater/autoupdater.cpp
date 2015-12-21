#include "autoupdater.h"
#include "autoupdater_p.h"
#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

AutoUpdater::AutoUpdater(QObject *parent) :
	QObject(parent),
	d_ptr(new AutoUpdaterPrivate(this))
{}

AutoUpdater::~AutoUpdater()
{
	delete this->d_ptr;
}

bool AutoUpdater::exitedNormally() const
{
	const Q_D(AutoUpdater);
	return d->normalExit;
}

int AutoUpdater::getErrorCode() const
{
	const Q_D(AutoUpdater);
	return d->lastErrorCode;
}

QByteArray AutoUpdater::getErrorLog() const
{
	const Q_D(AutoUpdater);
	return d->lastErrorLog;
}

QString AutoUpdater::maintenanceToolPath() const
{
	const Q_D(AutoUpdater);
	return d->mainInfo.toolPath;
}

bool AutoUpdater::isRunning() const
{
	const Q_D(AutoUpdater);
	return d->running;
}

QStringList AutoUpdater::updateArguments() const
{
	const Q_D(AutoUpdater);
	return d->mainInfo.runArgs;
}

bool AutoUpdater::runAsAdmin() const
{
	const Q_D(AutoUpdater);
	return d->mainInfo.runAdmin;
}

QList<AutoUpdater::UpdateInfo> AutoUpdater::updateInfo() const
{
	const Q_D(AutoUpdater);
	return d->updateInfos;
}

bool AutoUpdater::checkForUpdates()
{
	Q_D(AutoUpdater);
	if(d->running)
		return false;
	else {
		d->workingInfo = d->mainInfo;
		d->running = true;
		d->lastErrorCode = EXIT_SUCCESS;
		d->lastErrorLog.clear();

		QFileInfo toolInfo(QCoreApplication::applicationDirPath(), d->workingInfo.toolPath);
		d->mainProcess = new QProcess(this);
		d->mainProcess->setProgram(toolInfo.absoluteFilePath());
		d->mainProcess->setArguments({QStringLiteral("--checkupdates")});
		d->mainProcess->setWorkingDirectory(toolInfo.absolutePath());//TODO mac...

		connect(d->mainProcess, SELECT<int,QProcess::ExitStatus>::OVERLOAD_OF(&QProcess::finished),
				this, [this](int exitCode, QProcess::ExitStatus exitStatus){
			Q_D(AutoUpdater);
			d->updaterReady(exitCode, exitStatus);
		});
		connect(d->mainProcess, SELECT<QProcess::ProcessError>::OVERLOAD_OF(&QProcess::error),
				this, [this](const QProcess::ProcessError &error){
			Q_D(AutoUpdater);
			d->updaterError(error);
		});

		d->mainProcess->start(QIODevice::ReadOnly);
		return true;
	}
}

void AutoUpdater::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(AutoUpdater);
	d->mainInfo.toolPath = maintenanceToolPath;
}

void AutoUpdater::setUpdateArguments(QStringList updateArguments)
{
	Q_D(AutoUpdater);
	d->mainInfo.runArgs = updateArguments;
}

void AutoUpdater::setRunAsAdmin(bool runAsAdmin)
{
	Q_D(AutoUpdater);
	d->mainInfo.runAdmin = runAsAdmin;
}

//-------------PRIVATE IMPLEMENTATION---------------

AutoUpdaterPrivate::AutoUpdaterPrivate(AutoUpdater *q_ptr) :
	q_ptr(q_ptr),
	mainInfo(),
	updateInfos(),
	normalExit(true),
	lastErrorCode(EXIT_SUCCESS),
	lastErrorLog(),
	running(false),
	workingInfo(),
	mainProcess(NULL)
{
	//TODO test if ok on mac...
	this->mainInfo.toolPath = AutoUpdaterPrivate::toSystemExe(QStringLiteral("./maintenancetool"));
	this->mainInfo.runArgs = {QStringLiteral("--updater")};
	this->mainInfo.runAdmin = false;
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

void AutoUpdaterPrivate::updaterReady(int exitCode, QProcess::ExitStatus exitStatus)
{
	if(this->mainProcess) {
		if(exitStatus == QProcess::NormalExit) {
			this->normalExit = true;
			this->lastErrorCode = exitCode;
			this->lastErrorLog = this->mainProcess->readAllStandardError();
			QString updateOut = this->mainProcess->readAllStandardOutput();
			this->mainProcess->deleteLater();
			this->mainProcess = NULL;

			Q_Q(AutoUpdater);
			if(this->lastErrorCode != EXIT_SUCCESS) {
				this->running = false;
				emit q->checkUpdatesDone(false, true);
			} else {
				qDebug() << updateOut;
				this->running = false;
				emit q->checkUpdatesDone(false, false);
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
		emit q->checkUpdatesDone(false, true);
	}
}



AutoUpdater::UpdateInfo::UpdateInfo() :
	name(),
	version(),
	size(0)
{}

AutoUpdater::UpdateInfo::UpdateInfo(const AutoUpdater::UpdateInfo &other) :
	name(other.name),
	version(other.version),
	size(other.size)
{}

AutoUpdater::UpdateInfo::UpdateInfo(QString name, QVersionNumber version, quint64 size) :
	name(name),
	version(version),
	size(size)
{}
