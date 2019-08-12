#include "updater.h"
#include "updater_p.h"
#include "updaterplugin.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/private/qfactoryloader_p.h>

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logQtAutoUpdater, "QtAutoUpdater")

}

using namespace QtAutoUpdater;

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
						  (QtAutoUpdater_UpdaterPlugin_iid,
						   QLatin1String("/updaters")))

Updater::Updater(QObject *parent) :
	Updater(*new UpdaterPrivate{}, parent)
{}

Updater::Updater(UpdaterPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{
	Q_D(Updater);
	d->scheduler = new SimpleScheduler{this};

	QObjectPrivate::connect(qApp, &QCoreApplication::aboutToQuit,
							d, &UpdaterPrivate::_q_appAboutToExit,
							Qt::DirectConnection);
	connect(d->scheduler, &SimpleScheduler::scheduleTriggered,
			this, &Updater::checkForUpdates);
}

Updater *Updater::createUpdater(const QString &key, const QVariantMap &arguments, QObject *parent, AdminAuthoriser *authoriser)
{
	auto updater = new Updater{parent};
	auto backend = qLoadPlugin<UpdaterBackend, UpdaterPlugin>(loader, key, updater);
	if (!backend || !backend->initialize(arguments, authoriser)) {
		delete updater;
		return nullptr;
	}

	updater->d_func()->setBackend(backend);
	return updater;
}

Updater *Updater::createQtIfwUpdater(const QString &maintenancetoolPath, bool silent, QObject *parent, AdminAuthoriser *authoriser)
{
	return createUpdater(QStringLiteral("qtifw"), {
							 {QStringLiteral("path"), maintenancetoolPath},
							 {QStringLiteral("silent"), silent}
						 }, parent, authoriser);
}

Updater::~Updater()
{
	Q_D(Updater);
	if(d->runOnExit)
		qCWarning(logQtAutoUpdater) << "Updater destroyed with run on exit active before the application quit";
	if (d->running) {
		Q_UNIMPLEMENTED();
	}
}

UpdaterBackend *Updater::backend() const
{
	const Q_D(Updater);
	return d->backend;
}

bool Updater::willRunOnExit() const
{
	const Q_D(Updater);
	return d->runOnExit;
}

bool Updater::isRunning() const
{
	const Q_D(Updater);
	return d->running;
}

QList<UpdateInfo> Updater::updateInfo() const
{
	const Q_D(Updater);
	return d->updateInfos;
}

QString Updater::errorMessage() const
{
	const Q_D(Updater);
	return d->errorMsg;
}

int Updater::scheduleUpdate(int delaySeconds, bool repeated)
{
	if((static_cast<qint64>(delaySeconds) * 1000ll) > static_cast<qint64>(std::numeric_limits<int>::max())) {
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

bool Updater::runUpdater(bool forceOnExit)
{
	Q_D(Updater);
	if (d->backend->features().testFlag(UpdaterBackend::Feature::TriggerInstall)) {
		if (d->backend->features().testFlag(UpdaterBackend::Feature::InstallNeedsExit))
			forceOnExit = true;
		if (forceOnExit) {
			d->runOnExit = true;
			return true;
		} else
			return d->backend->triggerUpdates(d->updateInfos);
	} else
		return false;
}

void Updater::checkForUpdates()
{
	Q_D(Updater);
	if (!d->running) {
		d->running = true;
		d->updateInfos.clear();
		d->errorMsg.clear();
		emit updateInfoChanged(d->updateInfos, {});
		if (d->backend->features().testFlag(UpdaterBackend::Feature::CheckProgress))
			emit progressChanged(0.0, QStringLiteral(""), {}); // empty, but not null string
		else
			emit progressChanged(-1.0, tr("Checking for updates…"), {});
		emit runningChanged(d->running, {});
		d->backend->checkForUpdates();
	}
}

void Updater::abortUpdateCheck(int killDelay)
{
	Q_D(Updater);
	if(d->running) {
		if(killDelay != 0) {
			d->backend->abort(false);
			if(killDelay > 0) {
				QTimer::singleShot(killDelay, this, [this](){
					abortUpdateCheck(0);
				});
			}
		} else
			d->backend->abort(true);
	}
}

void Updater::cancelScheduledUpdate(int taskId)
{
	Q_D(Updater);
	d->scheduler->cancelSchedule(taskId);
}

void Updater::cancelExitRun()
{
	Q_D(Updater);
	d->runOnExit = false;
	d->adminAuth.reset();
}

// ------------- private implementation -------------

void UpdaterPrivate::setBackend(UpdaterBackend *newBackend)
{
	Q_Q(Updater);
	backend = newBackend;
	connect(backend, &UpdaterBackend::checkDone,
			this, &UpdaterPrivate::_q_checkDone);
	connect(backend, &UpdaterBackend::error,
			this, &UpdaterPrivate::_q_error);
	QObject::connect(backend, &UpdaterBackend::updateProgress,
					 q, std::bind(&Updater::progressChanged, q,
								  std::placeholders::_1,
								  std::placeholders::_2,
								  Updater::QPrivateSignal{}));
}

void UpdaterPrivate::_q_appAboutToExit()
{
	if (runOnExit) {
		runOnExit = false;
		backend->triggerUpdates(updateInfos);
	}
}

void UpdaterPrivate::_q_checkDone(QList<UpdateInfo> updates)
{
	Q_Q(Updater);
	updateInfos = std::move(updates);
	errorMsg.clear();
	running = false;
	emit q->runningChanged(running, {});
	if (updateInfos.isEmpty())
		emit q->checkUpdatesDone(Updater::Result::NoUpdates, {});
	else {
		emit q->updateInfoChanged(updateInfos, {});
		emit q->checkUpdatesDone(Updater::Result::NewUpdates, {});
	}
}

void UpdaterPrivate::_q_error(QString errorMessage)
{
	Q_Q(Updater);
	updateInfos.clear();
	errorMsg = std::move(errorMessage);
	running = false;
	emit q->runningChanged(running, {});
	emit q->checkUpdatesDone(Updater::Result::Error, {});
}

#include "moc_updater.cpp"
