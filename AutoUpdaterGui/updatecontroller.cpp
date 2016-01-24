#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include <QProgressBar>
#include <QCoreApplication>
#include <updatescheduler.h>
#include "messagemaster.h"
#include "adminauthorization.h"
#include "updatepanel.h"
using namespace QtAutoUpdater;

static void libInit()
{
	Q_INIT_RESOURCE(autoupdatergui_resource);
}
Q_COREAPP_STARTUP_FUNCTION(libInit)

UpdateController::UpdateController(QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, NULL))
{}

UpdateController::UpdateController(QWidget *parentWidget) :
	QObject(parentWidget),
	d_ptr(new UpdateControllerPrivate(this, parentWidget))
{}

UpdateController::UpdateController(const QString &maintenanceToolPath, QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, NULL))
{}

UpdateController::UpdateController(const QString &maintenanceToolPath, QWidget *parentWidget) :
	QObject(parentWidget),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, parentWidget))
{}

UpdateController::~UpdateController()
{
	delete this->d_ptr;
}

QAction *UpdateController::createUpdateAction(QObject *parent)
{
	QAction *updateAction = new QAction(QIcon(QStringLiteral(":/updaterIcons/update.ico")),
										tr("Check for Updates"),
										parent);
	updateAction->setMenuRole(QAction::ApplicationSpecificRole);
	updateAction->setToolTip(tr("Checks if new updates are available. You will be prompted before updates are installed."));
	updateAction->setIconVisibleInMenu(false);

	connect(updateAction, &QAction::triggered, this, [this](){
		this->start(UpdateController::ProgressLevel);
	});
	connect(this, &UpdateController::runningChanged,
			updateAction, &QAction::setDisabled);
	connect(this, &UpdateController::destroyed,
			updateAction, &QAction::deleteLater);

	return updateAction;
}

QWidget *UpdateController::createUpdatePanel(QWidget *parentWidget)
{
	return new UpdatePanel(this, parentWidget);
}

QString UpdateController::maintenanceToolPath() const
{
	const Q_D(UpdateController);
	return d->mainUpdater->maintenanceToolPath();
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
			d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : NULL);
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

const Updater * UpdateController::getUpdater() const
{
	const Q_D(UpdateController);
	return d->mainUpdater;
}

QWidget *UpdateController::parentWidget() const
{
	const Q_D(UpdateController);
	return d->window;
}

void UpdateController::setParent(QWidget *parent)
{
	Q_D(UpdateController);
	d->window = parent;
	d->infoDialog->setNewParent(parent);
	if(d->tProgress) {
		d->tProgress->hide();
		d->tProgress = NULL;
	}
	this->QObject::setParent(parent);
}

void UpdateController::setParent(QObject *parent)
{
	Q_D(UpdateController);
	d->window = NULL;
	d->infoDialog->setNewParent(NULL);
	if(d->tProgress) {
		d->tProgress->hide();
		d->tProgress = NULL;
	}
	this->QObject::setParent(parent);
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
		if(MessageMaster::question(d->window,
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
			MessageMaster::warning(d->window,
								   tr("Warning"),
								   tr("The program is already checking for updates!"));
		}
		d->running = false;
		emit runningChanged(false);
		return false;
	} else {
		if(d->displayLevel >= ProgressLevel) {
			d->checkUpdatesProgress = new ProgressDialog(d->window);
			connect(d->checkUpdatesProgress, &ProgressDialog::canceled, this, [d](){
				d->wasCanceled = true;
			});
			d->checkUpdatesProgress->open(d->mainUpdater, &QtAutoUpdater::Updater::abortUpdateCheck);
		}

		if(d->tProgress && d->displayLevel >= ExtendedInfoLevel) {
			d->tProgress->setRange(0, 0);
			d->tProgress->resume();
			d->tProgress->show();
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

int UpdateController::scheduleUpdate(UpdateTask *task, UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	int id = UpdateScheduler::instance()->scheduleTask(task);
	d->updateTasks.insert(id, displayLevel);
	return id;
}

void UpdateController::cancelScheduledUpdate(int taskId)
{
	UpdateScheduler::instance()->cancelTask(taskId);
}

void UpdateController::checkUpdatesDone(bool hasUpdates, bool hasError)
{
	Q_D(UpdateController);

	if(d->displayLevel >= ProgressLevel) {
		d->checkUpdatesProgress->hide();
		d->checkUpdatesProgress->deleteLater();
		d->checkUpdatesProgress = NULL;
	}
	if(d->wasCanceled) {
		if(d->displayLevel >= ExtendedInfoLevel) {
			if(d->tProgress) {
				d->tProgress->setRange(0, 1);
				d->tProgress->setValue(1);
				d->tProgress->pause();
			}
			MessageMaster::warning(d->window,
								   tr("Check for Updates"),
								   tr("Checking for updates was canceled!"));
			if(d->tProgress)
				d->tProgress->hide();
		}
	} else {
		if(hasUpdates) {
			if(d->displayLevel >= InfoLevel) {
				if(d->tProgress && d->displayLevel >= ExtendedInfoLevel){
					d->tProgress->setRange(0, 1);
					d->tProgress->setValue(1);
					d->tProgress->resume();
				}

				bool shouldShutDown = false;
				bool oldRunAdmin = d->runAdmin;
				UpdateInfoDialog::DialogResult res = d->infoDialog->showUpdateInfo(d->mainUpdater->updateInfo(),
																				   d->runAdmin,
																				   d->adminUserEdit);
				if(d->runAdmin != oldRunAdmin)
					emit runAsAdminChanged(d->runAdmin);

				switch(res) {
				case UpdateInfoDialog::InstallNow:
					shouldShutDown = true;
				case UpdateInfoDialog::InstallLater:
					d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : NULL);
					if(shouldShutDown)
						qApp->quit();
				case UpdateInfoDialog::NoInstall:
					break;
				default:
					Q_UNREACHABLE();
				}
			} else {
				d->mainUpdater->runUpdaterOnExit(d->runAdmin ? new AdminAuthorization() : NULL);
				if(d->displayLevel == ExitLevel) {
					MessageMaster::information(d->window,
											   tr("Install Updates"),
											   tr("New updates are available. The maintenance tool will be started to install those as soon as you close the application!"));
				} else
					qApp->quit();
			}
		} else {
			if(hasError) {
				qWarning() << "maintenancetool process finished with exit code"
						   << d->mainUpdater->getErrorCode()
						   << "and error string:"
						   << d->mainUpdater->getErrorLog();
			}

			if(d->displayLevel >= ExtendedInfoLevel) {
				if(d->mainUpdater->exitedNormally()) {
					if(d->tProgress) {
						d->tProgress->setRange(0, 1);
						d->tProgress->setValue(1);
						d->tProgress->stop();
					}
					MessageMaster::critical(d->window,
											tr("Check for Updates"),
											tr("No new updates available!"));
				} else {
					if(d->tProgress) {
						d->tProgress->setRange(0, 1);
						d->tProgress->setValue(1);
						d->tProgress->pause();
					}
					MessageMaster::warning(d->window,
										   tr("Warning"),
										   tr("The update process crashed!"));
				}
			}
		}

		if(d->tProgress && d->displayLevel >= ExtendedInfoLevel)
			d->tProgress->hide();
	}

	d->running = false;
	emit runningChanged(false);
}

void UpdateController::taskReady(int groupID)
{
	Q_D(UpdateController);
	if(d->updateTasks.contains(groupID))
		this->start(d->updateTasks.value(groupID));
}

void UpdateController::taskDone(int groupID)
{
	Q_D(UpdateController);
	d->updateTasks.remove(groupID);
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
	checkUpdatesProgress(NULL),
	wasCanceled(false),
	infoDialog(new UpdateInfoDialog(window)),
#ifdef Q_OS_WIN
	tButton(new QWinTaskbarButton(q_ptr)),
	tProgress(NULL),
#endif
	updateTasks()
{
	QObject::connect(this->mainUpdater, &Updater::checkUpdatesDone,
					 q_ptr, &UpdateController::checkUpdatesDone,
					 Qt::QueuedConnection);

	QObject::connect(UpdateScheduler::instance(), &UpdateScheduler::taskReady,
					 q_ptr, &UpdateController::taskReady,
					 Qt::QueuedConnection);
	QObject::connect(UpdateScheduler::instance(), &UpdateScheduler::taskFinished,
					 q_ptr, &UpdateController::taskDone,
					 Qt::QueuedConnection);

#ifdef Q_OS_WIN
	ShowEventFilter *evFilter = new ShowEventFilter(this);
	if(this->window) {
		if(this->window->window()->windowHandle()) {
			QEvent ev(QEvent::Show);
			evFilter->eventFilter(this->window->window(), &ev);
		} else
			this->window->window()->installEventFilter(evFilter);
	}
#endif
}

UpdateControllerPrivate::~UpdateControllerPrivate()
{
	if(this->running) {
		qWarning("UpdaterController destroyed while still running! "
				 "This may crash your application!");
	}

	this->infoDialog->deleteLater();
	for(int taskID : this->updateTasks.keys())
		UpdateScheduler::instance()->cancelTask(taskID);
}


#ifdef Q_OS_WIN
ShowEventFilter::ShowEventFilter(UpdateControllerPrivate *controller) :
	QObject(controller->q_ptr),
	controller(controller)
{}

bool ShowEventFilter::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::Show) {
		Q_ASSERT(dynamic_cast<QWidget*>(obj));
		QWidget *widget = static_cast<QWidget*>(obj);
		this->controller->tButton->setWindow(widget->windowHandle());
		this->controller->tProgress = this->controller->tButton->progress();
		this->deleteLater();
	}
	return false;
}
#endif
