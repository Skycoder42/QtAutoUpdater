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
	d_ptr(new UpdaterPrivate(this))
{
	Q_D(Updater);
	d->toolPath = UpdaterPrivate::toSystemExe(maintenanceToolPath);
}

Updater::~Updater() {}

bool Updater::exitedNormally() const
{
	const Q_D(Updater);
	return d->normalExit;
}

int Updater::errorCode() const
{
	const Q_D(Updater);
	return d->lastErrorCode;
}

QByteArray Updater::errorLog() const
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

int Updater::scheduleUpdate(int delaySeconds, bool repeated)
{
	if((((qint64)delaySeconds) * 1000ll) > (qint64)INT_MAX) {
		qCWarning(logQtAutoUpdater) << "delaySeconds to big to be converted to msecs";
		return 0;
	}
	Q_D(Updater);
	return d->scheduler->startSchedule(delaySeconds * 1000, repeated);
}

int Updater::scheduleUpdate(const QDateTime &when)
{
	Q_D(Updater);
	return d->scheduler->startSchedule(when);
}

void Updater::cancelScheduledUpdate(int taskId)
{
	Q_D(Updater);
	d->scheduler->cancelSchedule(taskId);
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
	debug << qPrintable(QStringLiteral("{Name: \"%1\"; Version: %2; Size: %3}")
						.arg(info.name)
						.arg(info.version.toString())
						.arg(info.size));
	return debug;
}
