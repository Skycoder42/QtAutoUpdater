#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include "updatebutton.h"
#include "installwizard_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <dialogmaster.h>

#include <QtAutoUpdaterCore/private/updater_p.h>

using namespace QtAutoUpdater;

UpdateController::UpdateController(QObject *parent) :
	UpdateController{nullptr, nullptr, parent}
{}

UpdateController::UpdateController(QWidget *parentWindow, QObject *parent) :
	UpdateController{nullptr, parentWindow, parent}
{}

UpdateController::UpdateController(Updater *updater, QObject *parent) :
	UpdateController{updater, nullptr, parent}
{}

UpdateController::UpdateController(Updater *updater, QWidget *parentWidget, QObject *parent) :
	QObject{*new UpdateControllerPrivate{}, parent}
{
	setParentWindow(parentWidget);
	setUpdater(updater);
}

UpdateController::~UpdateController()
{
	Q_D(UpdateController);
	if(d->running)
		qCWarning(logQtAutoUpdater) << "UpdaterController destroyed while still running! This can crash your application!";

	d->hideProgress();
}

QAction *UpdateController::createUpdateAction(Updater *updater, QObject *parent)
{
	auto updateAction = new QAction {
		UpdateControllerPrivate::getUpdatesIcon(),
		tr("Check for Updates"),
		parent
	};
	updateAction->setMenuRole(QAction::ApplicationSpecificRole);
	updateAction->setToolTip(tr("Checks if new updates are available. You will be prompted before updates are installed."));

	connect(updateAction, &QAction::triggered,
			updater, &Updater::checkForUpdates);
	connect(updater, &Updater::runningChanged,
			updateAction, &QAction::setDisabled);
	connect(updater, &Updater::destroyed,
			updateAction, std::bind(&QAction::setDisabled, updateAction, true));

	return updateAction;
}

QWidget *UpdateController::parentWindow() const
{
	Q_D(const UpdateController);
	return d->window;
}

bool UpdateController::isRunning() const
{
	Q_D(const UpdateController);
	return d->running;
}

UpdateController::DisplayLevel UpdateController::displayLevel() const
{
	Q_D(const UpdateController);
	return d->displayLevel;
}

QString UpdateController::desktopFileName() const
{
	Q_D(const UpdateController);
	return d->desktopFileName;
}

Updater *UpdateController::updater() const
{
	Q_D(const UpdateController);
	return d->updater;
}

void UpdateController::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	if (d->displayLevel == displayLevel)
		return;

	d->displayLevel = displayLevel;
	emit displayLevelChanged(d->displayLevel, {});
}

void UpdateController::setParentWindow(QWidget *parentWindow)
{
	Q_D(UpdateController);
	if (d->window == parentWindow)
		return;

	d->window = parentWindow;
	emit parentWindowChanged(d->window, {});
}

void UpdateController::setDesktopFileName(QString desktopFileName)
{
	Q_D(UpdateController);
	if (d->desktopFileName == desktopFileName)
		return;

	d->desktopFileName = std::move(desktopFileName);
	emit desktopFileNameChanged(d->desktopFileName, {});
}

void UpdateController::setUpdater(Updater *updater)
{
	Q_D(UpdateController);
	if (d->updater == updater)
		return;

	// cleanup old one
	if (d->updater) {
		d->updater->disconnect(this);
		if (d->updater->parent() == this)
			d->updater->deleteLater();
	}

	// setup new one
	d->updater = updater;
	if (updater) {
		d->updater->setParent(this);
		QObjectPrivate::connect(d->updater, &Updater::stateChanged,
								d, &UpdateControllerPrivate::_q_updaterStateChanged);
		QObjectPrivate::connect(d->updater, &Updater::showInstaller,
								d, &UpdateControllerPrivate::_q_showInstaller);
		d->_q_updaterStateChanged(d->updater->state());
	}
	emit updaterChanged(d->updater, {});
}

bool UpdateController::start()
{
	Q_D(UpdateController);
	if(d->running)
		return false;

	d->ensureRunning(true);

	// ask if updates should be checked
	if(d->displayLevel >= AskLevel) {
		if(DialogMaster::questionT(d->window,
								   tr("Check for Updates"),
								   tr("Do you want to check for updates now?"))
		   != QMessageBox::Yes) {
			d->ensureRunning(false);
			return false;
		}
	}

	// check for updates
	d->updater->checkForUpdates();
	return true;
}

bool UpdateController::start(DisplayLevel displayLevel)
{
	setDisplayLevel(displayLevel);
	return start();
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

//----------------- private implementation -----------------

QIcon UpdateControllerPrivate::getUpdatesIcon()
{
	const auto altPath = qEnvironmentVariable("QT_AUTOUPDATER_UPDATE_ICON",
											  QStringLiteral(":/QtAutoUpdater/icons/update.ico"));
	return QIcon::fromTheme(QStringLiteral("system-software-update"), QIcon{altPath});
}

void UpdateControllerPrivate::_q_updaterStateChanged(Updater::State state)
{
	switch (state) {
	case Updater::State::NoUpdates:
		enterNoUpdatesState();
		break;
	case Updater::State::Checking:
		enterCheckingState();
		break;
	case Updater::State::NewUpdates:
		enterNewUpdatesState();
		break;
	case Updater::State::Error:
		enterErrorState();
		break;
	case Updater::State::Installing:
		enterInstallingState();
		break;
	}
}

void UpdateControllerPrivate::_q_showInstaller(UpdateInstaller *installer)
{
	auto wizard = new InstallWizard{installer};
	wizard->show();
	wizard->raise();
	wizard->activateWindow();
}

void UpdateControllerPrivate::enterNoUpdatesState()
{
	hideProgress();
	if (showCanceled())
		return;

	if(running && displayLevel >= UpdateController::ExtendedInfoLevel) {
		DialogMaster::informationT(window,
								   UpdateController::tr("Check for Updates"),
								   UpdateController::tr("No new updates available!"));
	}
	ensureRunning(false);
}

void UpdateControllerPrivate::enterCheckingState()
{
	ensureRunning(true);
	if(displayLevel >= UpdateController::ProgressLevel && !checkUpdatesProgress) {
		checkUpdatesProgress = new ProgressDialog{desktopFileName, window};
		QObject::connect(checkUpdatesProgress.data(), &ProgressDialog::canceled, q_func(), [this](){
			wasCanceled = true;
		});
		checkUpdatesProgress->open(updater);
	}
}

void UpdateControllerPrivate::enterNewUpdatesState()
{
	ensureRunning(true);
	hideProgress();
	if (showCanceled())
		return;

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
			if(displayLevel >= UpdateController::ExitLevel) {
				DialogMaster::informationT(window,
										   UpdateController::tr("Install Updates"),
										   UpdateController::tr("New updates are available. The update tool will be "
											  "started to install those as soon as you close the application!"));
			} else
				qApp->quit();
		}
	}
	ensureRunning(false);
}

void UpdateControllerPrivate::enterErrorState()
{
	ensureRunning(true);
	hideProgress();
	if (showCanceled())
		return;

	if(displayLevel >= UpdateController::ExtendedInfoLevel) {
		DialogMaster::criticalT(window,
								UpdateController::tr("Check for Updates"),
								UpdateController::tr("An error occured while trying to check for updates!"));
	}
	ensureRunning(false);
}

void UpdateControllerPrivate::enterInstallingState()
{
	// nothing for now
}

void UpdateControllerPrivate::ensureRunning(bool newState)
{
	if (running != newState) {
		Q_Q(UpdateController);
		running = newState;
		if (!running)
			wasCanceled = false;
		emit q->runningChanged(running, {});
	}
}

void UpdateControllerPrivate::hideProgress()
{
	if(checkUpdatesProgress) {
		checkUpdatesProgress->hide();  // explicitly hide so child dialogs are NOT shown on top of it
		checkUpdatesProgress->deleteLater();
		checkUpdatesProgress.clear();
	}
}

bool UpdateControllerPrivate::showCanceled()
{
	if(wasCanceled) {
		if(displayLevel >= UpdateController::ExtendedInfoLevel) {
			DialogMaster::warningT(window,
								   UpdateController::tr("Check for Updates"),
								   UpdateController::tr("Checking for updates was canceled!"));
		}
		ensureRunning(false);
		return true;
	} else
		return false;
}

#include "moc_updatecontroller.cpp"
