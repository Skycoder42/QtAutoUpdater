#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include <QMessageBox>
#include <QProgressBar>
using namespace QtAutoUpdater;

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
		d->checkUpdatesProgress->open(d->mainUpdater,
									  &QtAutoUpdater::Updater::abortUpdateCheck,
									  5000);
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

void UpdateController::abort()
{

}

void UpdateController::checkUpdatesDone(bool hasUpdates, bool hasError)
{
	Q_D(UpdateController);
	if(hasUpdates) {
		if(d->displayLevel >= ProgressLevel) {
			d->checkUpdatesProgress->hide();
			d->checkUpdatesProgress->deleteLater();
			d->checkUpdatesProgress = NULL;
			QMessageBox::information(d->window,
									 tr("Check for Updates"),
									 tr("New updates available!"));
		}

		d->running = false;
		emit runningChanged(false);
	} else {
		qDebug() << "hasError:" << hasError
				 << "\nexitedNormally:" << d->mainUpdater->exitedNormally()
				 << "\nerrorCode:" << d->mainUpdater->getErrorCode()
				 << "\nerrorLog:" << d->mainUpdater->getErrorLog();
//		if(hasError) {
//			//TODO how to distinguish ok from error...
//		} else {
			if(d->displayLevel >= ProgressLevel) {
				d->checkUpdatesProgress->hide();
				d->checkUpdatesProgress->deleteLater();
				d->checkUpdatesProgress = NULL;
				QMessageBox::information(d->window,
										 tr("Check for Updates"),
										 tr("No new updates available!"));
			}
//		}

		d->running = false;
		emit runningChanged(false);
	}
}

//-----------------PRIVATE IMPLEMENTATION-----------------

UpdateControllerPrivate::UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window) :
	q_ptr(q_ptr),
	window(window),
	displayLevel(UpdateController::InfoLevel),
	running(false),
	mainUpdater(new Updater(NULL)),
	checkUpdatesProgress(NULL)
{
	QObject::connect(this->mainUpdater, &Updater::checkUpdatesDone,
					 q_ptr, &UpdateController::checkUpdatesDone,
					 Qt::QueuedConnection);
}

UpdateControllerPrivate::~UpdateControllerPrivate()
{
	delete this->mainUpdater;
}
