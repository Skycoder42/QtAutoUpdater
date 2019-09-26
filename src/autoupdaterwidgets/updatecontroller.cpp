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

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logController, "qt.autoupdater.widgets.UpdateController")

}

UpdateController::UpdateController(QObject *parent) :
	UpdateController{nullptr, parent}
{}

UpdateController::UpdateController(QWidget *parentWindow) :
	UpdateController{nullptr, parentWindow}
{}

UpdateController::UpdateController(Updater *updater, QObject *parent) :
	QObject{*new UpdateControllerPrivate{}, parent}
{
	setUpdater(updater);
}

UpdateController::UpdateController(Updater *updater, QWidget *parentWidget) :
	QObject{*new UpdateControllerPrivate{}, parentWidget}
{
	setUpdater(updater);
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
	return qobject_cast<QWidget*>(parent());
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

Updater::InstallScope UpdateController::installScope() const
{
	Q_D(const UpdateController);
	return d->installScope;
}

bool UpdateController::isDetailedUpdateInfo() const
{
	Q_D(const UpdateController);
	return d->detailedUpdateInfo;
}

void UpdateController::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	if (d->displayLevel == displayLevel)
		return;

	d->displayLevel = displayLevel;
	emit displayLevelChanged(d->displayLevel, {});
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
		d->cleanUp();
		if (d->updater->parent() == this)
			d->updater->deleteLater();
	}

	// setup new one
	d->updater = updater;
	if (d->updater) {
		QObjectPrivate::connect(d->updater, &Updater::stateChanged,
								d, &UpdateControllerPrivate::_q_updaterStateChanged,
								Qt::QueuedConnection);
		QObjectPrivate::connect(d->updater, &Updater::showInstaller,
								d, &UpdateControllerPrivate::_q_showInstaller);
		QObjectPrivate::connect(d->updater, &Updater::destroyed,
								d, &UpdateControllerPrivate::_q_updaterDestroyed);
		d->_q_updaterStateChanged(d->updater->state());
	}
	emit updaterChanged(d->updater, {});
}

void UpdateController::setInstallScope(Updater::InstallScope installScope)
{
	Q_D(UpdateController);
	if (d->installScope == installScope)
		return;

	d->installScope = installScope;
	emit installScopeChanged(d->installScope, {});
}

void UpdateController::setDetailedUpdateInfo(bool detailedUpdateInfo)
{
	Q_D(UpdateController);
	if (d->detailedUpdateInfo == detailedUpdateInfo)
		return;

	d->detailedUpdateInfo = detailedUpdateInfo;
	emit detailedUpdateInfoChanged(d->detailedUpdateInfo, {});
}

bool UpdateController::start()
{
	Q_D(UpdateController);
	if(!d->updater || d->updater->isRunning())
		return false;

	// ask if updates should be checked
	if(d->canShow(DisplayLevel::Ask)) {
		if(DialogMaster::questionT(parentWindow(),
								   tr("Check for Updates"),
								   tr("Do you want to check for updates now?"))
		   != QMessageBox::Yes) {
			return false;
		}
	} else if (d->displayLevel == DisplayLevel::Ask)
		return false;  // special case: user "ignored" the dialog -> means false

	// check for updates
	d->updater->checkForUpdates();
	return true;
}

bool UpdateController::start(DisplayLevel displayLevel)
{
	setDisplayLevel(displayLevel);
	return start();
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
	// is possible, because queued connection
	if (!updater)
		return;

	switch (state) {
	case Updater::State::NoUpdates:
		enterNoUpdatesState();
		break;
	case Updater::State::Checking:
		enterCheckingState();
		break;
	case Updater::State::Canceling:
		enterCancelingState();
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

	wasChecking = (state == Updater::State::Checking);
	wasCanceled = (state == Updater::State::Canceling);
}

void UpdateControllerPrivate::_q_showInstaller(UpdateInstaller *installer)
{
	auto wizard = new InstallWizard{installer};
	wizard->show();
	wizard->raise();
	wizard->activateWindow();
}

void UpdateControllerPrivate::_q_updaterDestroyed()
{
	Q_Q(UpdateController);
	cleanUp();
	emit q->updaterChanged(nullptr, {});
}

void UpdateControllerPrivate::enterNoUpdatesState()
{
	Q_Q(UpdateController);
	hideProgress();
	if (showCanceled())
		return;

	if (wasChecking) {
		if(canShow(DisplayLevel::ExtendedInfo)) {
			DialogMaster::informationT(q->parentWindow(),
									   UpdateController::tr("Check for Updates"),
									   UpdateController::tr("No new updates available!"));
		}
	}
}

void UpdateControllerPrivate::enterCheckingState()
{
	Q_Q(UpdateController);
	if(!checkUpdatesProgress && canShow(DisplayLevel::Progress)) {
		checkUpdatesProgress = new ProgressDialog{desktopFileName, q->parentWindow()};
		checkUpdatesProgress->open(updater);
	}
}

void UpdateControllerPrivate::enterCancelingState()
{
	if(checkUpdatesProgress)
		checkUpdatesProgress->setCanceled();
}

void UpdateControllerPrivate::enterNewUpdatesState()
{
	Q_Q(UpdateController);
	hideProgress();
	if (showCanceled())
		return;

	if(canShow(DisplayLevel::Info)) {
		const auto updateInfos = updater->updateInfo();
		const auto res = detailedUpdateInfo ?
							 UpdateInfoDialog::showUpdateInfo(updateInfos,
															  desktopFileName,
															  updater->backend()->features(),
															  updater->backend()->secondaryInfo(),
															  q->parentWindow()) :
							 UpdateInfoDialog::showSimpleInfo(updateInfos,
															  updater->backend()->features(),
															  q->parentWindow());

		auto installStarted = true;
		switch(res) {
		case UpdateInfoDialog::InstallNow:
			installStarted = updater->runUpdater(Updater::InstallModeFlag::Parallel, installScope);
			if (installStarted && updater->willRunOnExit())
				qApp->quit();
			break;
		case UpdateInfoDialog::InstallLater:
			installStarted = updater->runUpdater(Updater::InstallModeFlag::ForceOnExit, installScope);
			break;
		case UpdateInfoDialog::NoInstall:
			break;
		default:
			Q_UNREACHABLE();
		}

		if (!installStarted) {
			DialogMaster::criticalT(q->parentWindow(),
									UpdateController::tr("Install Updates"),
									UpdateController::tr("Failed to start the installer to install new updates!"));
		}
	} else if (displayLevel <= UpdateController::DisplayLevel::Exit) {
		updater->runUpdater();
		if (updater->willRunOnExit()) {
			if(canShow(DisplayLevel::Exit)) {
				DialogMaster::informationT(q->parentWindow(),
										   UpdateController::tr("Install Updates"),
										   UpdateController::tr("New updates are available. The update tool will be "
											  "started to install those as soon as you close the application!"));
			} else if (displayLevel == UpdateController::DisplayLevel::Automatic)
				qApp->quit();
		}
	}
}

void UpdateControllerPrivate::enterErrorState()
{
	Q_Q(UpdateController);
	hideProgress();
	if (showCanceled())
		return;

	if(canShow(DisplayLevel::ExtendedInfo)) {
		DialogMaster::criticalT(q->parentWindow(),
								UpdateController::tr("Check for Updates"),
								UpdateController::tr("An error occured while trying to check for updates!"));
	}
}

void UpdateControllerPrivate::enterInstallingState()
{
	// nothing for now
}

bool UpdateControllerPrivate::canShow(DisplayLevel level) const
{
	Q_Q(const UpdateController);
	if (displayLevel < level)
		return false;
	auto pWindow = q->parentWindow();
	return pWindow ? pWindow->isVisible() || pWindow->isMinimized() : true;
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
	Q_Q(UpdateController);
	if(wasCanceled) {
		wasCanceled = false;
		if(canShow(DisplayLevel::ExtendedInfo)) {
			DialogMaster::warningT(q->parentWindow(),
								   UpdateController::tr("Check for Updates"),
								   UpdateController::tr("Checking for updates was canceled!"));
		}
		return true;
	} else
		return false;
}

void UpdateControllerPrivate::cleanUp()
{
	hideProgress();
	wasCanceled = false;
	wasChecking = false;
}

#include "moc_updatecontroller.cpp"
