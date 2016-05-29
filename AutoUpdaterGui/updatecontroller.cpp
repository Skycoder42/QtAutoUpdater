#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include <QProgressBar>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <dialogmaster.h>
#include "adminauthorization.h"
#include "updatebutton.h"
using namespace QtAutoUpdater;

static void libInit()
{
	Q_INIT_RESOURCE(autoupdatergui_resource);
}
Q_COREAPP_STARTUP_FUNCTION(libInit)

UpdateController::UpdateController(QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, nullptr))
{}

UpdateController::UpdateController(QWidget *parentWidget, QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, parentWidget))
{}

UpdateController::UpdateController(const QString &maintenanceToolPath, QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, nullptr))
{}

UpdateController::UpdateController(const QString &maintenanceToolPath, QWidget *parentWidget, QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, parentWidget))
{}

UpdateController::~UpdateController(){}

QAction *UpdateController::createUpdateAction(QObject *parent)
{
	QAction *updateAction = new QAction(QIcon(QStringLiteral(":/QtAutoUpdater/icons/update.ico")),
										tr("Check for Updates"),
										parent);
	updateAction->setMenuRole(QAction::ApplicationSpecificRole);
	updateAction->setToolTip(tr("Checks if new updates are available. You will be prompted before updates are installed."));

	connect(updateAction, &QAction::triggered, this, [this](){
		this->start(UpdateController::ProgressLevel);
	});
	connect(this, &UpdateController::runningChanged,
			updateAction, &QAction::setDisabled);
	connect(this, &UpdateController::destroyed,
			updateAction, &QAction::deleteLater);

	return updateAction;
}

QString UpdateController::maintenanceToolPath() const
{
	const Q_D(UpdateController);
	return d->mainUpdater->maintenanceToolPath();
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

bool UpdateController::runAsAdmin() const
{
	const Q_D(UpdateController);
	return d->runAdmin;
}

void UpdateController::setRunAsAdmin(bool runAsAdmin, bool userEditable)
{
	Q_D(UpdateController);
	if(d->runAdmin != runAsAdmin) {
		d->runAdmin = runAsAdmin;
		if(d->mainUpdater->willRunOnExit())
			d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : nullptr);
		emit runAsAdminChanged(runAsAdmin);
	}
	d->adminUserEdit = userEditable;
}

QStringList UpdateController::updateRunArgs() const
{
	const Q_D(UpdateController);
	return d->runArgs;
}

void UpdateController::setUpdateRunArgs(QStringList updateRunArgs)
{
	Q_D(UpdateController);
	d->runArgs = updateRunArgs;
}

void UpdateController::resetUpdateRunArgs()
{
	Q_D(UpdateController);
	d->runArgs = QStringList(QStringLiteral("--updater"));
}

bool UpdateController::isDetailedUpdateInfo() const
{
	const Q_D(UpdateController);
	return d->detailedInfo;
}

void UpdateController::setDetailedUpdateInfo(bool detailedUpdateInfo)
{
	Q_D(UpdateController);
	d->detailedInfo = detailedUpdateInfo;
}

const Updater * UpdateController::updater() const
{
	const Q_D(UpdateController);
	return d->mainUpdater;
}

bool UpdateController::start(DisplayLevel displayLevel)
{
	Q_D(UpdateController);

	if(d->running)
		return false;
	d->running = true;
	emit runningChanged(true);
	d->wasCanceled = false;
	d->displayLevel = displayLevel;

	if(d->displayLevel >= AskLevel) {
		if(DialogMaster::questionT(d->window,
								   tr("Check for Updates"),
								   tr("Do you want to check for updates now?"))
		   != QMessageBox::Yes) {
			d->running = false;
			emit runningChanged(false);
			return false;
		}
	}

	if(!d->mainUpdater->checkForUpdates()) {
		if(d->displayLevel >= ProgressLevel) {
			DialogMaster::warningT(d->window,
								   tr("Check for Updates"),
								   tr("The program is already checking for updates!"));
		}
		d->running = false;
		emit runningChanged(false);
		return false;
	} else {
		if(d->displayLevel >= ExtendedInfoLevel) {
			d->checkUpdatesProgress = new ProgressDialog(d->window);
			if(d->displayLevel >= ProgressLevel) {
                connect(d->checkUpdatesProgress.data(), &ProgressDialog::canceled, this, [d](){
					d->wasCanceled = true;
				});
				d->checkUpdatesProgress->open(d->mainUpdater, &QtAutoUpdater::Updater::abortUpdateCheck);
			}
		}

		return true;
	}
}

bool UpdateController::cancelUpdate(int maxDelay)
{
	Q_D(UpdateController);
	if(d->mainUpdater->isRunning()) {
		d->wasCanceled = true;
		if(d->checkUpdatesProgress)
			d->checkUpdatesProgress->setCanceled();
		d->mainUpdater->abortUpdateCheck(maxDelay, true);
		return true;
	} else
		return false;
}

int UpdateController::scheduleUpdate(int delaySeconds, bool repeated, UpdateController::DisplayLevel displayLevel)
{
	if((((qint64)delaySeconds) * 1000) > INT_MAX) {
		qCWarning(logQtAutoUpdater) << "delaySeconds to big to be converted to msecs";
		return 0;
	}
	Q_D(UpdateController);
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

void UpdateController::checkUpdatesDone(bool hasUpdates, bool hasError)
{
	Q_D(UpdateController);

	if(d->displayLevel >= ExtendedInfoLevel) {
		QMessageBox::Icon iconType = QMessageBox::NoIcon;
		if(hasUpdates)
			iconType = QMessageBox::Information;
		else {
			if(d->wasCanceled || !d->mainUpdater->exitedNormally())
				iconType = QMessageBox::Warning;
			else
				iconType = QMessageBox::Critical;
		}

		if(d->checkUpdatesProgress) {
			d->checkUpdatesProgress->hide(iconType);
			d->checkUpdatesProgress->deleteLater();
			d->checkUpdatesProgress = nullptr;
		}
	}
	if(d->wasCanceled) {
		if(d->displayLevel >= ExtendedInfoLevel) {
			DialogMaster::warningT(d->window,
								   tr("Check for Updates"),
								   tr("Checking for updates was canceled!"));
		}
	} else {
		if(hasUpdates) {
			if(d->displayLevel >= InfoLevel) {
				bool shouldShutDown = false;
				bool oldRunAdmin = d->runAdmin;
				UpdateInfoDialog::DialogResult res = UpdateInfoDialog::showUpdateInfo(d->mainUpdater->updateInfo(),
																					  d->runAdmin,
																					  d->adminUserEdit,
																					  d->detailedInfo,
																					  d->window);
				if(d->runAdmin != oldRunAdmin)
					emit runAsAdminChanged(d->runAdmin);

				switch(res) {
				case UpdateInfoDialog::InstallNow:
					shouldShutDown = true;
				case UpdateInfoDialog::InstallLater:
					d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : nullptr);
					if(shouldShutDown)
						qApp->quit();
				case UpdateInfoDialog::NoInstall:
					break;
				default:
					Q_UNREACHABLE();
				}
			} else {
				d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : nullptr);
				if(d->displayLevel == ExitLevel) {
					DialogMaster::informationT(d->window,
											   tr("Install Updates"),
											   tr("New updates are available. The maintenance tool will be "
												  "started to install those as soon as you close the application!"));
				} else
					qApp->quit();
			}
		} else {
			if(hasError) {
				qCWarning(logQtAutoUpdater) << "maintenancetool process finished with exit code"
										   << d->mainUpdater->errorCode()
										   << "and error string:"
										   << d->mainUpdater->errorLog();
			}

			if(d->displayLevel >= ExtendedInfoLevel) {
				if(d->mainUpdater->exitedNormally()) {
					DialogMaster::criticalT(d->window,
											tr("Check for Updates"),
											tr("No new updates available!"));
				} else {
					DialogMaster::warningT(d->window,
										   tr("Check for Updates"),
										   tr("The update process crashed!"));
				}
			}
		}
	}

	d->running = false;
	emit runningChanged(false);
}

void UpdateController::timerTriggered(const QVariant &parameter)
{
	if(parameter.canConvert<DisplayLevel>())
		this->start(parameter.value<DisplayLevel>());
}

//-----------------PRIVATE IMPLEMENTATION-----------------

UpdateControllerPrivate::UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window) :
	UpdateControllerPrivate(q_ptr, QString(), window)
{}

UpdateControllerPrivate::UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window) :
	q_ptr(q_ptr),
	window(window),
	displayLevel(UpdateController::InfoLevel),
	running(false),
	mainUpdater(toolPath.isEmpty() ? new Updater(q_ptr) : new Updater(toolPath, q_ptr)),
	runAdmin(true),
	adminUserEdit(true),
	runArgs(QStringLiteral("--updater")),
	detailedInfo(true),
	checkUpdatesProgress(nullptr),
	wasCanceled(false),
	scheduler(new SimpleScheduler(q_ptr))
{
	QObject::connect(this->mainUpdater, &Updater::checkUpdatesDone,
					 q_ptr, &UpdateController::checkUpdatesDone,
					 Qt::QueuedConnection);
	QObject::connect(this->scheduler, &SimpleScheduler::scheduleTriggered,
					 q_ptr, &UpdateController::timerTriggered);

#ifdef Q_OS_UNIX
	QFileInfo maintenanceInfo(QCoreApplication::applicationDirPath(),
                              this->mainUpdater->maintenanceToolPath());
	this->runAdmin = (maintenanceInfo.ownerId() == 0);
#endif
}

UpdateControllerPrivate::~UpdateControllerPrivate()
{
	if(this->running) {
		qCWarning(logQtAutoUpdater, "UpdaterController destroyed while still running! "
									"This can crash your application!");
	}

	if(this->checkUpdatesProgress)
		this->checkUpdatesProgress->deleteLater();
}
