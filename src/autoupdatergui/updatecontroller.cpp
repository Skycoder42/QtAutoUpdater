#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include "updatebutton.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <dialogmaster.h>

#include <QtAutoUpdaterCore/private/updater_p.h>

using namespace QtAutoUpdater;

UpdateController::UpdateController(Updater *updater, QObject *parent) :
	UpdateController{updater, nullptr, parent}
{}

UpdateController::UpdateController(Updater *updater, QWidget *parentWidget, QObject *parent) :
	QObject{*new UpdateControllerPrivate{}, parent}
{
	Q_D(UpdateController);
	d->updater = updater;
	d->updater->setParent(this);
	d->window = parentWidget;

	d->scheduler = new SimpleScheduler{this};

	QObjectPrivate::connect(d->updater, &Updater::checkUpdatesDone,
							d, &UpdateControllerPrivate::_q_updaterCheckDone);
	QObjectPrivate::connect(d->scheduler, &SimpleScheduler::scheduleTriggered,
							d, &UpdateControllerPrivate::_q_timerTriggered);
}

UpdateController::~UpdateController()
{
	Q_D(UpdateController);
	if(d->running)
		qCWarning(logQtAutoUpdater) << "UpdaterController destroyed while still running! This can crash your application!";

	if(d->checkUpdatesProgress)
		d->checkUpdatesProgress->deleteLater();
}

QAction *UpdateController::createUpdateAction(QObject *parent)
{
	return createUpdateAction(ProgressLevel, parent);
}

QAction *UpdateController::createUpdateAction(DisplayLevel displayLevel, QObject *parent)
{
	auto updateAction = new QAction {
		UpdateControllerPrivate::getUpdatesIcon(),
		tr("Check for Updates"),
		parent
	};
	updateAction->setMenuRole(QAction::ApplicationSpecificRole);
	updateAction->setToolTip(tr("Checks if new updates are available. You will be prompted before updates are installed."));

	connect(updateAction, &QAction::triggered,
			this, std::bind(&UpdateController::start, this, displayLevel));
	connect(this, &UpdateController::runningChanged,
			updateAction, &QAction::setDisabled);
	connect(this, &UpdateController::destroyed,
			updateAction, &QAction::deleteLater);

	return updateAction;
}

QWidget *UpdateController::parentWindow() const
{
	const Q_D(UpdateController);
	return d->window;
}

void UpdateController::setParentWindow(QWidget *parentWindow)
{
	Q_D(UpdateController);
	d->window = parentWindow;
}

UpdateController::DisplayLevel UpdateController::currentDisplayLevel() const
{
	const Q_D(UpdateController);
	return d->displayLevel;
}

bool UpdateController::isRunning() const
{
	const Q_D(UpdateController);
	return d->running;
}

QString UpdateController::desktopFileName() const
{
	const Q_D(UpdateController);
	return d->desktopFileName;
}

void UpdateController::setDesktopFileName(QString desktopFileName)
{
	Q_D(UpdateController);
	d->desktopFileName = std::move(desktopFileName);
}

Updater *UpdateController::updater() const
{
	const Q_D(UpdateController);
	return d->updater;
}

bool UpdateController::start(DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	if(d->running)
		return false;

	d->wasCanceled = false;
	d->displayLevel = displayLevel;
	d->running = true;
	emit runningChanged(true, {});

	// ask if updates should be checked
	if(d->displayLevel >= AskLevel) {
		if(DialogMaster::questionT(d->window,
								   tr("Check for Updates"),
								   tr("Do you want to check for updates now?"))
		   != QMessageBox::Yes) {
			d->running = false;
			emit runningChanged(false, {});
			return false;
		}
	}

	// abort early if the updater is already running
	if(d->updater->isRunning()) {
		if(d->displayLevel >= ExtendedInfoLevel) {
			DialogMaster::warningT(d->window,
								   tr("Check for Updates"),
								   tr("The program is already checking for updates!"));
		}
		d->running = false;
		emit runningChanged(false, {});
		return false;
	} else {
		// otherwise start the update check and display the progress if wished
		if(d->displayLevel >= ProgressLevel) {
			d->checkUpdatesProgress = new ProgressDialog{d->desktopFileName, d->window};
			connect(d->checkUpdatesProgress.data(), &ProgressDialog::canceled, this, [this](){
				Q_D(UpdateController);
				d->wasCanceled = true;
			});
			d->checkUpdatesProgress->open(d->updater);
		}

		d->updater->checkForUpdates();
		return true;
	}
}

bool UpdateController::cancelUpdate(int maxDelay)
{
	Q_D(UpdateController);
	if(d->updater->state() == Updater::State::Checking) {
		d->wasCanceled = true;
		if(d->checkUpdatesProgress)
			d->checkUpdatesProgress->setCanceled();
		d->updater->abortUpdateCheck(maxDelay);
		return true;
	} else
		return false;
}

int UpdateController::scheduleUpdate(int delaySeconds, bool repeated, UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	if((static_cast<qint64>(delaySeconds) * 1000ll) > static_cast<qint64>(std::numeric_limits<int>::max())) {
		qCWarning(logQtAutoUpdater) << "delaySeconds to big to be converted to msecs";
		return 0;
	}
	return d->scheduler->startSchedule(delaySeconds * 1000, repeated, QVariant::fromValue(displayLevel));
}

int UpdateController::scheduleUpdate(const QDateTime &when, UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	return d->scheduler->startSchedule(when, QVariant::fromValue(displayLevel));
}

void UpdateController::cancelScheduledUpdate(int taskId)
{
	Q_D(UpdateController);
	d->scheduler->cancelSchedule(taskId);
}

void UpdateControllerPrivate::_q_updaterCheckDone(Updater::State state)
{
	Q_Q(UpdateController);
	if (!running)
		return;

	if(displayLevel >= UpdateController::ProgressLevel) {
		if(checkUpdatesProgress) {
			checkUpdatesProgress->hide();  // explicitly hide so child dialogs are NOT shown on top of it
			checkUpdatesProgress->deleteLater();
			checkUpdatesProgress.clear();
		}
	}

	if(wasCanceled) {
		if(displayLevel >= UpdateController::ExtendedInfoLevel) {
			DialogMaster::warningT(window,
								   UpdateController::tr("Check for Updates"),
								   UpdateController::tr("Checking for updates was canceled!"));
		}
	} else {
		switch (state) {
		case Updater::State::NewUpdates:
			if(displayLevel >= UpdateController::InfoLevel) {
				const auto updateInfos = updater->updateInfo();
				const auto res = UpdateInfoDialog::showUpdateInfo(updateInfos,
																  desktopFileName,
																  updater->backend()->features(),
																  window);

				switch(res) {
				case UpdateInfoDialog::InstallNow:
					updater->runUpdater(false);
					if (updater->willRunOnExit())
						qApp->quit();
					break;
				case UpdateInfoDialog::InstallLater:
					updater->runUpdater(true);
					break;
				case UpdateInfoDialog::NoInstall:
					break;
				default:
					Q_UNREACHABLE();
				}
			} else {
				updater->runUpdater(false);
				if (updater->willRunOnExit()) {
					if(displayLevel == UpdateController::ExitLevel) {
						DialogMaster::informationT(window,
												   UpdateController::tr("Install Updates"),
												   UpdateController::tr("New updates are available. The update tool will be "
													  "started to install those as soon as you close the application!"));
					} else
						qApp->quit();
				}
			}
			break;
		case Updater::State::NoUpdates:
			if(displayLevel >= UpdateController::ExtendedInfoLevel) {
				DialogMaster::informationT(window,
										   UpdateController::tr("Check for Updates"),
										   UpdateController::tr("No new updates available!"));
			}
			break;
		case Updater::State::Error:
			if(displayLevel >= UpdateController::ExtendedInfoLevel) {
				DialogMaster::criticalT(window,
										UpdateController::tr("Check for Updates"),
										UpdateController::tr("An error occured while trying to check for updates!"));
			}
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}

	running = false;
	emit q->runningChanged(false, {});
}

void UpdateControllerPrivate::_q_timerTriggered(const QVariant &parameter)
{
	Q_Q(UpdateController);
	if(parameter.canConvert<UpdateController::DisplayLevel>())
		q->start(parameter.value<UpdateController::DisplayLevel>());
}

//----------------- private implementation -----------------

QIcon UpdateControllerPrivate::getUpdatesIcon()
{
	const auto altPath = qEnvironmentVariable("QT_AUTOUPDATER_UPDATE_ICON",
											  QStringLiteral(":/QtAutoUpdater/icons/update.ico"));
	return QIcon::fromTheme(QStringLiteral("system-software-update"), QIcon{altPath});
}

#include "moc_updatecontroller.cpp"
