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

void AutoUpdater::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(AutoUpdater);
	d->toolPath = maintenanceToolPath;
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
