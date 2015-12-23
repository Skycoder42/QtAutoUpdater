#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include <QMessageBox>
#include <QProgressBar>
#include <QCoreApplication>
using namespace QtAutoUpdater;

static void libInit()
{
	Q_INIT_RESOURCE(autoupdaterwidgets_resource);
}
Q_COREAPP_STARTUP_FUNCTION(libInit)

UpdateController::UpdateController(QWidget *parentWindow) :
	QObject(parentWindow),
	d_ptr(new UpdateControllerPrivate(this, parentWindow))
{}

UpdateController::~UpdateController()
{
	delete this->d_ptr;
}

QString UpdateController::maintenanceToolPath() const
{
	const Q_D(UpdateController);
	return d->mainUpdater->maintenanceToolPath();
}

void UpdateController::setMaintenanceToolPath(QString maintenanceToolPath)
{
	Q_D(UpdateController);
	d->mainUpdater->setMaintenanceToolPath(maintenanceToolPath);
}

UpdateController::DisplayLevel UpdateController::displayLevel() const
{
	const Q_D(UpdateController);
	return d->displayLevel;
}

void UpdateController::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateController);
	d->displayLevel = displayLevel;
}

bool UpdateController::isRunning() const
{
	const Q_D(UpdateController);
	return d->running;// || d->mainUpdater->isRunning();
}

bool UpdateController::start()
{
	Q_D(UpdateController);

	if(d->running)
		return false;
	d->running = true;
	emit runningChanged(true);
	d->wasCanceled = false;

	if(d->displayLevel >= AskLevel) {
		if(QMessageBox::question(d->window,
								 tr("Check for Updates"),
								 tr("Do you want to check for updates now?"))
		   != QMessageBox::Yes) {
			d->running = false;
			emit runningChanged(false);
			return false;
		}
	}

	if(d->displayLevel >= ProgressLevel) {
		d->checkUpdatesProgress = new ProgressDialog(d->window);
		connect(d->checkUpdatesProgress, &ProgressDialog::canceled, this, [d](){
			d->wasCanceled = true;
		});
		d->checkUpdatesProgress->open(d->mainUpdater, &QtAutoUpdater::Updater::abortUpdateCheck);
	}

	if(!d->mainUpdater->checkForUpdates()) {
		if(d->displayLevel >= ProgressLevel) {
			d->checkUpdatesProgress->hide();
			d->checkUpdatesProgress->deleteLater();
			d->checkUpdatesProgress = NULL;
			QMessageBox::warning(d->window,
								 tr("Warning"),
								 tr("The program is already checking for updates!"));
		}
		d->running = false;
		emit runningChanged(false);
		return false;
	} else
		return true;
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

void UpdateController::checkUpdatesDone(bool hasUpdates, bool hasError)
{
	Q_D(UpdateController);

	if(d->displayLevel >= ProgressLevel) {
		d->checkUpdatesProgress->hide();
		d->checkUpdatesProgress->deleteLater();
		d->checkUpdatesProgress = NULL;
	}
	if(d->wasCanceled) {
		QMessageBox::warning(d->window,
							 tr("Check for Updates"),
							 tr("Checking for updates was canceled!"));
	} else {
		if(hasUpdates) {
			if(d->displayLevel >= InfoLevel) {
				bool shouldShutDown = false;
				switch(d->infoDialog->showUpdateInfo(d->mainUpdater->updateInfo())) {
				case UpdateInfoDialog::InstallNow:
					shouldShutDown = true;
				case UpdateInfoDialog::InstallLater:
					d->mainUpdater->runUpdaterOnExit();
					if(shouldShutDown)
						qApp->quit();
				case UpdateInfoDialog::NoInstall:
					break;
				default:
					Q_UNREACHABLE();
				}
			} else {
				d->mainUpdater->runUpdaterOnExit();
				if(d->displayLevel == ExitLevel) {
					QMessageBox::information(d->window,
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
				if(!d->mainUpdater->exitedNormally()) {
					QMessageBox::warning(d->window,
										 tr("Warning"),
										 tr("The update process crashed!"));
				}
			}

			if(d->mainUpdater->exitedNormally()){
				if(d->displayLevel >= ProgressLevel) {
					QMessageBox::critical(d->window,
										  tr("Check for Updates"),
										  tr("No new updates available!"));
				}
			}
		}
	}

	d->running = false;
	emit runningChanged(false);
}

//-----------------PRIVATE IMPLEMENTATION-----------------

UpdateControllerPrivate::UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window) :
	q_ptr(q_ptr),
	window(window),
	displayLevel(UpdateController::InfoLevel),
	running(false),
	mainUpdater(new Updater(NULL)),
	checkUpdatesProgress(NULL),
	wasCanceled(false),
	infoDialog(new UpdateInfoDialog(window))
{
	QObject::connect(this->mainUpdater, &Updater::checkUpdatesDone,
					 q_ptr, &UpdateController::checkUpdatesDone,
					 Qt::QueuedConnection);
}

UpdateControllerPrivate::~UpdateControllerPrivate()
{
	delete this->mainUpdater;
}
