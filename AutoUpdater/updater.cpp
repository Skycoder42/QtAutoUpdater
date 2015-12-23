#include "updater.h"
#include "updater_p.h"
using namespace QtAutoUpdater;

Updater::Updater(QObject *parent) :
	QObject(parent),
	d_ptr(new UpdaterPrivate(this))
{}

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

QString Updater::maintenanceToolPath() const
{
	const Q_D(Updater);
	return d->toolPath;
}

void Updater::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(Updater);
	d->toolPath = UpdaterPrivate::toSystemExe(maintenanceToolPath);
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

int  Updater::scheduleUpdate(qint64 delay, bool repeated)
{
	if(delay <= 0)
		return 0;
	Q_D(Updater);
	int tId = d->startTimer(delay * 1000, Qt::VeryCoarseTimer);
	d->activeTimers.insert(tId, repeated);
	return tId;
}

bool Updater::cancelScheduledUpdate(int taskId)
{
	Q_D(Updater);
	d->killTimer(taskId);
	return (d->activeTimers.remove(taskId) > 0);
}

void Updater::runUpdaterOnExit(const QStringList &arguments)
{
	Q_D(Updater);
	d->runOnExit = true;
	d->runArguments = arguments;
}

void Updater::cancelExitRun()
{
	Q_D(Updater);
	d->runOnExit = false;
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
