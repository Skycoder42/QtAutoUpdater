#include "updater.h"
#include "updater_p.h"
#include "updaterplugin.h"
#include "updateinstaller.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>
#include <QtCore/private/qfactoryloader_p.h>
using namespace QtAutoUpdater;
using namespace std::chrono;

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logUpdater, "qt.autoupdater.core.Updater")

}

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
						  (QtAutoUpdater_UpdaterPlugin_iid,
						   QLatin1String("/updaters")))

Updater::Updater(QObject *parent) :
	Updater(*new UpdaterPrivate{}, parent)
{}

Updater::Updater(UpdaterPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{
	qRegisterMetaType<State>();

	Q_D(Updater);
	d->scheduler = new SimpleScheduler{this};

	QObjectPrivate::connect(qApp, &QCoreApplication::aboutToQuit,
							d, &UpdaterPrivate::_q_appAboutToExit,
							Qt::DirectConnection);
	connect(d->scheduler, &SimpleScheduler::scheduleTriggered,
			this, qOverload<>(&Updater::checkForUpdates));

	connect(this, &Updater::stateChanged,
			this, [this](State state) {
		switch (state) {
		case State::NoUpdates:
		case State::NewUpdates:
		case State::Error:
			emit runningChanged(false, {});
			break;
		case State::Checking:
		case State::Canceling:
		case State::Installing:
			emit runningChanged(true, {});
			break;
		}
	});
}

Updater *Updater::create(QObject *parent)
{
	auto config = UpdaterPrivate::findDefaultConfig();
	if (config)
		return create(config, parent);
	else {
		qCCritical(logUpdater) << "Unable to find the default updater configuration file";
		return nullptr;
	}
}

Updater *Updater::create(const QString &configPath, QObject *parent)
{
	return create(new QSettings {
							 configPath,
							 QSettings::IniFormat
						 }, parent);
}

Updater *Updater::create(QSettings *config, QObject *parent)
{
	return UpdaterPrivate::createUpdater(new SettingsConfigReader {config}, parent);
}

Updater *Updater::create(QString key, QVariantMap arguments, QObject *parent)
{
	return UpdaterPrivate::createUpdater(new VariantConfigReader {
											 std::move(key),
											 std::move(arguments)
										 }, parent);
}

Updater *Updater::create(UpdaterBackend::IConfigReader *configReader, QObject *parent)
{
	return UpdaterPrivate::createUpdater(configReader, parent);
}

Updater::~Updater()
{
	Q_D(Updater);
	if (isRunning())
		qCWarning(logUpdater) << "Destroyed while still running. This can lead to corruption of your application!";
	else if(d->runOnExit)
		qCWarning(logUpdater) << "Destroyed with run on exit active before the application quit";
}

UpdaterBackend *Updater::backend() const
{
	Q_D(const Updater);
	return d->backend;
}

UpdaterBackend::Features Updater::features() const
{
	Q_D(const Updater);
	return d->backend->features();
}

bool Updater::willRunOnExit() const
{
	Q_D(const Updater);
	return d->runOnExit;
}

Updater::State Updater::state() const
{
	Q_D(const Updater);
	return d->state;
}

bool Updater::isRunning() const
{
	Q_D(const Updater);
	return d->state == State::Checking ||
			d->state == State::Canceling ||
			d->state == State::Installing;
}

QList<UpdateInfo> Updater::updateInfo() const
{
	Q_D(const Updater);
	return d->updateInfos;
}

int Updater::scheduleUpdate(std::chrono::seconds delaySeconds, bool repeated)
{
	Q_D(Updater);
	return d->scheduler->startSchedule(duration_cast<milliseconds>(delaySeconds), repeated);
}

int Updater::scheduleUpdate(int delaySeconds, bool repeated)
{
	return scheduleUpdate(seconds{delaySeconds}, repeated);
}

int Updater::scheduleUpdate(const QDateTime &when)
{
	Q_D(Updater);
	return d->scheduler->startSchedule(when);
}

bool Updater::runUpdater(bool forceOnExit)
{
	Q_D(Updater);
	if (isRunning())
		return false;

	if (!d->backend->features().testFlag(UpdaterBackend::Feature::ParallelInstall))
		forceOnExit = true;

	if (d->backend->features().testFlag(UpdaterBackend::Feature::TriggerInstall)) {
		if (forceOnExit) {
			if (!d->runOnExit) {
				d->runOnExit = true;
				emit runOnExitChanged(d->runOnExit, {});
			}
			return true;
		} else {
			const auto ok = d->backend->triggerUpdates(d->updateInfos, true);
			if (ok) {
				d->state = State::Installing;
				emit stateChanged(d->state, {});
			}
			return ok;
		}
	} else if (d->backend->features().testFlag(UpdaterBackend::Feature::PerformInstall)) {
		if (forceOnExit) {
			qCCritical(d->backend->logCat()) << "Backend does not support installation after exiting";
			return false;
		} else {
			auto installer = d->backend->createInstaller();
			if (installer)  {
				connect(installer, &UpdateInstaller::installSucceeded,
						this, [this, installer]() {
					installer->disconnect(this);
					d_func()->_q_triggerInstallDone(true);
				});
				connect(installer, &UpdateInstaller::installFailed,
						this, [this, installer]() {
					installer->disconnect(this);
					d_func()->_q_triggerInstallDone(false);
				});
				connect(installer, &UpdateInstaller::destroyed,
						this, [this, installer]() {
					installer->disconnect(this);
					d_func()->_q_triggerInstallDone(false);
				});
				installer->setComponents(d->updateInfos);

				d->state = State::Installing;
				emit stateChanged(d->state, {});
				emit showInstaller(installer, {});
				return true;
			} else
				return false;
		}
	} else {
		qCCritical(d->backend->logCat()) << "Backend does not support installation";
		return false;
	}
}

void Updater::checkForUpdates()
{
	Q_D(Updater);
	if (isRunning())
		return;

	d->state = State::Checking;
	d->updateInfos.clear();
	emit updateInfoChanged(d->updateInfos, {});
	if (d->backend->features().testFlag(UpdaterBackend::Feature::CheckProgress))
		emit progressChanged(0.0, QStringLiteral(""), {}); // empty, but not null string
	else
		emit progressChanged(-1.0, tr("Checking for updates…"), {});
	emit stateChanged(d->state, {});
	d->backend->checkForUpdates();
}

void Updater::abortUpdateCheck(int killDelay)
{
	Q_D(Updater);
	if(d->state == State::Checking) {
		d->state = State::Canceling;
		emit stateChanged(d->state, {});

		if(killDelay != 0) {
			d->backend->abort(false);
			if(killDelay > 0) {
				QTimer::singleShot(killDelay, this, [d](){
					if (d->state == State::Canceling)
						d->backend->abort(true);
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
	if (d->runOnExit) {
		d->runOnExit = false;
		emit runOnExitChanged(d->runOnExit, {});
	}
}

// ------------- private implementation -------------

QSettings *UpdaterPrivate::findDefaultConfig()
{
#ifdef Q_OS_WIN
	// Windows only: try the registry as first and thus preferred location
	{
		auto conf = new QSettings {
			QSettings::NativeFormat,
			QSettings::UserScope,
			QCoreApplication::organizationName(),
			QCoreApplication::applicationName()
		};
		conf->beginGroup(QStringLiteral("updater"));
		if (conf->contains(QStringLiteral("backend")))
			return conf;
		else
			delete conf;
	}
#endif

	// try config directories first
	auto paths = QStandardPaths::locateAll(QStandardPaths::AppConfigLocation, QStringLiteral("updater.conf"));
	// then try data dirs (includes bundle/exe root etc., depending on the platform)
	paths += QStandardPaths::locateAll(QStandardPaths::AppDataLocation, QStringLiteral("updater.conf"));
	for (const auto &path : QStandardPaths::locateAll(QStandardPaths::AppConfigLocation, QStringLiteral("updater.conf"))) {
		const auto conf = new QSettings{path, QSettings::IniFormat};
		if (conf->contains(QStringLiteral("backend")))
			return conf;
		else
			delete conf;
	}

	return nullptr;
}

Updater *UpdaterPrivate::createUpdater(UpdaterBackend::IConfigReader *config, QObject *parent)
{
	auto updater = new Updater{parent};
	auto backend = qLoadPlugin<UpdaterBackend, UpdaterPlugin>(loader, config->backend(), updater);
	if (!backend || !backend->initialize(QScopedPointer<UpdaterBackend::IConfigReader>{config})) {
		delete updater;
		return nullptr;
	}

	updater->d_func()->setupBackend(backend);
	return updater;
}

void UpdaterPrivate::setupBackend(UpdaterBackend *newBackend)
{
	Q_Q(Updater);
	Q_ASSERT(!backend);
	backend = newBackend;
	connect(backend, &UpdaterBackend::checkDone,
			this, &UpdaterPrivate::_q_checkDone);
	QObject::connect(backend, &UpdaterBackend::checkProgress,
					 q, std::bind(&Updater::progressChanged, q,
								  std::placeholders::_1,
								  std::placeholders::_2,
								  Updater::QPrivateSignal{}));
	connect(backend, &UpdaterBackend::triggerInstallDone,
			this, &UpdaterPrivate::_q_triggerInstallDone);
}

void UpdaterPrivate::_q_appAboutToExit()
{
	if (runOnExit) {
		runOnExit = false;
		backend->triggerUpdates(updateInfos, false);
	}
}

void UpdaterPrivate::_q_checkDone(bool success, QList<UpdateInfo> updates)
{
	Q_Q(Updater);
	if (success) {
		if (state == State::Canceling)
			state = State::NoUpdates;
		else {
			updateInfos = std::move(updates);
			if (updateInfos.isEmpty())
				state = State::NoUpdates;
			else {
				state = State::NewUpdates;
				emit q->updateInfoChanged(updateInfos, {});
			}
		}
	} else {
		updateInfos.clear();
		state = State::Error;
	}
	emit q->stateChanged(state, {});
	emit q->checkUpdatesDone(state, {});
}

void UpdaterPrivate::_q_triggerInstallDone(bool success)
{
	Q_Q(Updater);
	if (state == State::Installing) {
		updateInfos.clear();
		state = success ? State::NoUpdates : State::Error;
		emit q->stateChanged(state, {});
		emit q->installDone(success, {});
	}
}



VariantConfigReader::VariantConfigReader(QString &&backend, QVariantMap &&map) :
	_backend{std::move(backend)},
	_map{std::move(map)}
{}

QString VariantConfigReader::backend() const {
	return _backend;
}

std::optional<QVariant> VariantConfigReader::value(const QString &key) const {
	return _map.contains(key) ? _map.value(key) : std::optional<QVariant>{std::nullopt};
}

QVariant VariantConfigReader::value(const QString &key, const QVariant &defaultValue) const {
	return _map.value(key, defaultValue);
}



SettingsConfigReader::SettingsConfigReader(QSettings *settings) :
	_settings{settings}
{}

QString SettingsConfigReader::backend() const {
	return _settings->value(QStringLiteral("backend")).toString();
}

std::optional<QVariant> SettingsConfigReader::value(const QString &key) const {
	return _settings->contains(key) ? _settings->value(key) : std::optional<QVariant>{std::nullopt};
}

QVariant SettingsConfigReader::value(const QString &key, const QVariant &defaultValue) const {
	return _settings->value(key, defaultValue);
}

#include "moc_updater.cpp"
