#include "updatecontroller.h"
#include "updatecontroller_p.h"
#include "updatepanel.h"
#include <QMessageBox>
#include <QProgressBar>
#include <QCoreApplication>
using namespace QtAutoUpdater;

static int showMessage(QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text)
{
	QMessageBox::StandardButtons btns;
	switch(icon) {
	case QMessageBox::Question:
		btns = QMessageBox::Yes | QMessageBox::No;
		break;
	default:
		btns = QMessageBox::Ok;
		break;
	}

	QMessageBox msgBox(icon, title, text, btns, parent);
#if defined(Q_OS_OSX)
	msgBox.setWindowModality(Qt::WindowModal);
#endif

	return msgBox.exec();
}

static void libInit()
{
	Q_INIT_RESOURCE(autoupdaterwidgets_resource);
}
Q_COREAPP_STARTUP_FUNCTION(libInit)

UpdateController::UpdateController(QObject *parent) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, NULL))
{}

UpdateController::UpdateController(QWidget *parentWindow) :
	QObject(parentWindow),
	d_ptr(new UpdateControllerPrivate(this, parentWindow))
{}

UpdateController::UpdateController(QObject *parent, const QString &maintenanceToolPath) :
	QObject(parent),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, NULL))
{}

UpdateController::UpdateController(QWidget *parentWindow, const QString &maintenanceToolPath) :
	QObject(parentWindow),
	d_ptr(new UpdateControllerPrivate(this, maintenanceToolPath, parentWindow))
{}

UpdateController::~UpdateController()
{
	delete this->d_ptr;
}

QAction *UpdateController::getUpdateAction() const
{
	const Q_D(UpdateController);
	return d->updateAction;
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

Updater *UpdateController::getUpdater() const
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
		if(showMessage(d->window,
					   QMessageBox::Question,
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
			showMessage(d->window,
						QMessageBox::Warning,
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
		if(d->displayLevel >= ExtendedInfoLevel) {
			showMessage(d->window,
						QMessageBox::Warning,
						tr("Check for Updates"),
						tr("Checking for updates was canceled!"));
		}
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
					showMessage(d->window,
								QMessageBox::Information,
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
					showMessage(d->window,
								QMessageBox::Warning,
								tr("Warning"),
								tr("The update process crashed!"));
				}
			}

			if(d->mainUpdater->exitedNormally()){
				if(d->displayLevel >= ExtendedInfoLevel) {
					showMessage(d->window,
								QMessageBox::Critical,
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
	UpdateControllerPrivate(q_ptr, QString(), window)
{}

UpdateControllerPrivate::UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window) :
	q_ptr(q_ptr),
	window(window),
	updateAction(new QAction(QIcon(QStringLiteral(":/updaterIcons/update.ico")),
							 UpdateController::tr("Check for Updates"),
							 q_ptr)),
	displayLevel(UpdateController::InfoLevel),
	running(false),
	mainUpdater(toolPath.isEmpty() ? new Updater(q_ptr) : new Updater(toolPath, q_ptr)),
	checkUpdatesProgress(NULL),
	wasCanceled(false),
	infoDialog(new UpdateInfoDialog(window))
{
	this->updateAction->setMenuRole(QAction::ApplicationSpecificRole);
	this->updateAction->setToolTip(UpdateController::tr("Checks if new updates are available. You will be prompted before updates are installed."));
	this->updateAction->setIconVisibleInMenu(false);

	QObject::connect(this->mainUpdater, &Updater::checkUpdatesDone,
					 q_ptr, &UpdateController::checkUpdatesDone,
					 Qt::QueuedConnection);

	QObject::connect(this->updateAction, &QAction::triggered, q_ptr, [this](){
		this->q_ptr->start(UpdateController::ProgressLevel);
	});
	QObject::connect(this->q_ptr, &UpdateController::runningChanged,
					 this->updateAction, &QAction::setDisabled);
}

UpdateControllerPrivate::~UpdateControllerPrivate()
{
	delete this->mainUpdater;
}
