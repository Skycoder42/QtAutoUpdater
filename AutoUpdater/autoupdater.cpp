#include "autoupdater.h"
#include "autoupdater_p.h"

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
	return d->toolPath;
}

void AutoUpdater::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(AutoUpdater);
	d->toolPath = maintenanceToolPath;
}

bool AutoUpdater::isRunning() const
{
	const Q_D(AutoUpdater);
	return d->running;
}

QList<AutoUpdater::UpdateInfo> AutoUpdater::updateInfo() const
{
	const Q_D(AutoUpdater);
	return d->updateInfos;
}

bool AutoUpdater::checkForUpdates()
{
	Q_D(AutoUpdater);
	return d->startUpdateCheck();
}

void AutoUpdater::abortUpdateCheck(int maxDelay)
{
	Q_D(AutoUpdater);
	d->stopUpdateCheck(maxDelay);
}

int AutoUpdater::scheduleUpdate(qint64 mDelay, bool repeated, Qt::TimerType timerType)
{
	Q_D(AutoUpdater);
	int tId = d->startTimer(mDelay, timerType);
	d->activeTimers.insert(tId, repeated);
	return tId;
}

bool AutoUpdater::cancelScheduledUpdate(int taskId)
{
	Q_D(AutoUpdater);
	d->killTimer(taskId);
	return (d->activeTimers.remove(taskId) > 0);
}

void AutoUpdater::runUpdaterOnExit(const QStringList &arguments, bool runAsAdmin)
{
	Q_D(AutoUpdater);
	d->runExit(arguments, runAsAdmin);
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

QDebug &operator<<(QDebug &debug, const AutoUpdater::UpdateInfo &info)
{
	debug << qPrintable(QStringLiteral("{Name: \"%1\"; Version: %2; Size: %3}")
						.arg(info.name)
						.arg(info.version.toString())
						.arg(info.size));
	return debug;
}
