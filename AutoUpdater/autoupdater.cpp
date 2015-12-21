#include "autoupdater.h"
#include "autoupdater_p.h"
#include <QProcess>
#include <QCoreApplication>
#include <QFileInfo>

AutoUpdater::AutoUpdater(QObject *parent) :
	QObject(parent),
	d_ptr(new AutoUpdaterPrivate(this))
{}

AutoUpdater::~AutoUpdater()
{
	delete this->d_ptr;
}

int AutoUpdater::getErrorCode() const
{
	const Q_D(AutoUpdater);
	return d->lastErrorCode;
}

QString AutoUpdater::getErrorLog() const
{
	const Q_D(AutoUpdater);
	return d->lastErrorLog;
}

QString AutoUpdater::maintenanceToolPath() const
{
	const Q_D(AutoUpdater);
	return d->toolPath;
}

bool AutoUpdater::isRunning() const
{
	const Q_D(AutoUpdater);
	return d->watcher->isRunning();
}

QStringList AutoUpdater::updateArguments() const
{
	const Q_D(AutoUpdater);
	return d->runArgs;
}

bool AutoUpdater::runAsAdmin() const
{
	const Q_D(AutoUpdater);
	return d->runAdmin;
}

QList<AutoUpdater::UpdateInfo> AutoUpdater::updateInfo() const
{
	const Q_D(AutoUpdater);
	return d->infos;
}

bool AutoUpdater::checkForUpdates(int delay)
{
	Q_D(AutoUpdater);
	if(d->watcher->isRunning())
		return false;
	else {
		d->watcher->setFuture(QtConcurrent::run(&AutoUpdaterPrivate::runUpdateChecker,
												d->createWorkingCopy(),
												delay));
		return true;
	}
}

void AutoUpdater::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(AutoUpdater);
	d->toolPath = maintenanceToolPath;
}

void AutoUpdater::setUpdateArguments(QStringList updateArguments)
{
	Q_D(AutoUpdater);
	d->runArgs = updateArguments;
}

void AutoUpdater::setRunAsAdmin(bool runAsAdmin)
{
	Q_D(AutoUpdater);
	d->runAdmin = runAsAdmin;
}

//-------------PRIVATE IMPLEMENTATION---------------

AutoUpdaterPrivate::AutoUpdaterPrivate(AutoUpdater *q_ptr) :
	q_ptr(q_ptr),
	running(false),
	mainInfo(),
	updateInfos(),
	lastErrorCode(EXIT_SUCCESS),
	lastErrorLog(),
	workingInfo(),
	waitTimer(new QTimer(this->q_ptr)),
	mainProcess(NULL)
{
	//TODO test if ok on mac...
	this->mainInfo.toolPath = AutoUpdaterPrivate::toSystemExe(QStringLiteral("./maintenancetool"));
	this->mainInfo.runArgs = QStringLiteral("--updater");
	this->mainInfo.runAdmin = false;

	this->waitTimer->setSingleShot(true);
	//HERE
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

AutoUpdaterPrivate *AutoUpdaterPrivate::createWorkingCopy() const
{
	AutoUpdaterPrivate *cpy = new AutoUpdaterPrivate(this->q_ptr);
	cpy->toolPath = this->toolPath;
	cpy->runArgs = this->runArgs;
	cpy->runAdmin = this->runAdmin;
	cpy->toolPath = this->toolPath;
	return cpy;
}

//int AutoUpdaterPrivate::runUpdateChecker(AutoUpdaterPrivate *workingCopy, int delay)
//{
//	QFileInfo toolInfo(QCoreApplication::applicationDirPath(), workingCopy->toolPath);
//	if(delay > 0)
//		QThread::msleep(delay);//TODO better use QTimer

//	QProcess toolProcess;
//	toolProcess.setProgram(toolInfo.absoluteFilePath());
//	toolProcess.setArguments(QStringLiteral("--checkupdates"));
//	toolProcess.setWorkingDirectory(toolInfo.absolutePath());//TODO mac...

//	toolProcess.start(QIODevice::ReadOnly);
//	toolProcess.waitForFinished(-1);
//}
