#include "updater.h"
#include "updater_p.h"
#include "updatescheduler.h"
using namespace QtAutoUpdater;

Updater::Updater(QObject *parent) :
	Updater(QStringLiteral("./maintenancetool"), parent)//TODO path on mac...
{}

Updater::Updater(const QString &maintenanceToolPath, QObject *parent) :
	QObject(parent),
	d_ptr(new UpdaterPrivate(this))
{
	Q_D(Updater);
	d->toolPath = UpdaterPrivate::toSystemExe(maintenanceToolPath);
}

Updater::~Updater()
{
	delete this->d_ptr;
}

bool Updater::exitedNormally() const
{
	const Q_D(Updater);
	return d->normalExit;
}

int Updater::getErrorCode() const
{
	const Q_D(Updater);
	return d->lastErrorCode;
}

QByteArray Updater::getErrorLog() const
{
	const Q_D(Updater);
	return d->lastErrorLog;
}

bool Updater::willRunOnExit() const
{
	const Q_D(Updater);
	return d->runOnExit;
}

QString Updater::maintenanceToolPath() const
{
	const Q_D(Updater);
	return d->toolPath;
}

bool Updater::isRunning() const
{
	const Q_D(Updater);
	return d->running;
}

QList<Updater::UpdateInfo> Updater::updateInfo() const
{
	const Q_D(Updater);
	return d->updateInfos;
}

bool Updater::checkForUpdates()
{
	Q_D(Updater);
	return d->startUpdateCheck();
}

void Updater::abortUpdateCheck(int maxDelay, bool async)
{
	Q_D(Updater);
	d->stopUpdateCheck(maxDelay, async);
}

int Updater::scheduleUpdate(UpdateTask *task)
{
	Q_D(Updater);
	UpdateScheduler::instance()->start();
	int id = UpdateScheduler::instance()->scheduleTask(task);
	d->updateTasks.append(id);
	return id;
}

void Updater::cancelScheduledUpdate(int taskId)
{
	UpdateScheduler::instance()->cancelTaskGroup(taskId);
}

void Updater::runUpdaterOnExit(const QStringList &arguments, AdminAuthoriser *authoriser)
{
	Q_D(Updater);
	d->runOnExit = true;
	d->runArguments = arguments;
	d->adminAuth.reset(authoriser);
}

void Updater::cancelExitRun()
{
	Q_D(Updater);
	d->runOnExit = false;
	d->adminAuth.reset();
}



Updater::UpdateInfo::UpdateInfo() :
	name(),
	version(),
	size(0)
{}

Updater::UpdateInfo::UpdateInfo(const Updater::UpdateInfo &other) :
	name(other.name),
	version(other.version),
	size(other.size)
{}

Updater::UpdateInfo::UpdateInfo(QString name, QVersionNumber version, quint64 size) :
	name(name),
	version(version),
	size(size)
{}

QDebug &operator<<(QDebug &debug, const Updater::UpdateInfo &info)
{
	debug << qPrintable(QStringLiteral("{Name: \"%1\"; Version: %2; Size: %3}")
						.arg(info.name)
						.arg(info.version.toString())
						.arg(info.size));
	return debug;
}
