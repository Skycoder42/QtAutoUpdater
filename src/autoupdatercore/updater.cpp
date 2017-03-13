#include "updater.h"
#include "updater_p.h"

#include <QtCore/QDebug>

using namespace QtAutoUpdater;

#ifdef Q_OS_OSX
#define DEFAULT_TOOL_PATH QStringLiteral("../../maintenancetool")
#else
#define DEFAULT_TOOL_PATH QStringLiteral("./maintenancetool")
#endif

Updater::Updater(QObject *parent) :
	Updater(DEFAULT_TOOL_PATH, parent)
{}

Updater::Updater(const QString &maintenanceToolPath, QObject *parent) :
	QObject(parent),
	d(new UpdaterPrivate(this))
{
	d->toolPath = UpdaterPrivate::toSystemExe(maintenanceToolPath);
}

Updater::~Updater() {}

bool Updater::exitedNormally() const
{
	return d->normalExit;
}

int Updater::errorCode() const
{
	return d->lastErrorCode;
}

QByteArray Updater::errorLog() const
{
	return d->lastErrorLog;
}

bool Updater::willRunOnExit() const
{
	return d->runOnExit;
}

QString Updater::maintenanceToolPath() const
{
	return d->toolPath;
}

bool Updater::isRunning() const
{
	return d->running;
}

QList<Updater::UpdateInfo> Updater::updateInfo() const
{
	return d->updateInfos;
}

bool Updater::checkForUpdates()
{
	return d->startUpdateCheck();
}

void Updater::abortUpdateCheck(int maxDelay, bool async)
{
	d->stopUpdateCheck(maxDelay, async);
}

int Updater::scheduleUpdate(int delaySeconds, bool repeated)
{
	if((((qint64)delaySeconds) * 1000ll) > (qint64)INT_MAX) {
		qCWarning(logQtAutoUpdater) << "delaySeconds to big to be converted to msecs";
		return 0;
	}
	return d->scheduler->startSchedule(delaySeconds * 1000, repeated);
}

int Updater::scheduleUpdate(const QDateTime &when)
{
	return d->scheduler->startSchedule(when);
}

void Updater::cancelScheduledUpdate(int taskId)
{
	d->scheduler->cancelSchedule(taskId);
}

void Updater::runUpdaterOnExit(AdminAuthoriser *authoriser)
{
	runUpdaterOnExit({QStringLiteral("--updater")}, authoriser);
}

void Updater::runUpdaterOnExit(const QStringList &arguments, AdminAuthoriser *authoriser)
{
	d->runOnExit = true;
	d->runArguments = arguments;
	d->adminAuth.reset(authoriser);
}

void Updater::cancelExitRun()
{
	d->runOnExit = false;
	d->adminAuth.reset();
}



Updater::UpdateInfo::UpdateInfo() :
	name(),
	version(),
	size(0ull)
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
	QDebugStateSaver state(debug);
	Q_UNUSED(state);

	debug.noquote() << QStringLiteral("{Name: \"%1\"; Version: %2; Size: %3}")
					   .arg(info.name)
					   .arg(info.version.toString())
					   .arg(info.size);
	return debug;
}
