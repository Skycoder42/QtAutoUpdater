#include "updatebutton.h"
#include "updatebutton_p.h"
#include "ui_updatebutton.h"
#include "updatecontroller.h"
#include "updater.h"
using namespace QtAutoUpdater;

UpdateButton::UpdateButton(QWidget *parent, UpdateController *controller) :
	QWidget(parent),
	d_ptr(new UpdateButtonPrivate(this, controller))
{}

UpdateButton::~UpdateButton(){}

QString UpdateButton::animationFile() const
{
	const Q_D(UpdateButton);
	return d->loadingGif->fileName();
}

bool UpdateButton::isShowingResult() const
{
	const Q_D(UpdateButton);
	return d->showResult;
}

UpdateController::DisplayLevel UpdateButton::displayLevel() const
{
	const Q_D(UpdateButton);
	return d->level;
}

UpdateController *UpdateButton::controller() const
{
	const Q_D(UpdateButton);
	return d->controller;
}

void UpdateButton::resetState()
{
	Q_D(UpdateButton);
	d->loadingGif->setPaused(true);
	d->ui->loaderLabel->setVisible(false);
	d->ui->statusLabel->setVisible(false);
	d->ui->checkButton->setEnabled(true);
}

void UpdateButton::setAnimationFile(QString animationFile)
{
	Q_D(UpdateButton);
	d->loadingGif->setFileName(animationFile);
}

void UpdateButton::setAnimationDevice(QIODevice *animationDevice)
{
	Q_D(UpdateButton);
	d->loadingGif->setDevice(animationDevice);
}

void UpdateButton::resetAnimationFile()
{
	Q_D(UpdateButton);
	d->loadingGif->setFileName(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"));
}

void UpdateButton::setShowResult(bool showResult)
{
	Q_D(UpdateButton);
	d->showResult = showResult;
}

void UpdateButton::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdateButton);
	d->level = displayLevel;
}

bool UpdateButton::setController(UpdateController *controller)
{
	Q_D(UpdateButton);
	if(d->loadingGif->state() != QMovie::Running) {
		d->updateController(controller);
		this->resetState();
		emit controllerChanged(controller);
		return true;
	} else
		return false;
}

void UpdateButton::startUpdate()
{
	Q_D(UpdateButton);
	if(!d->controller.isNull())
		d->controller->start(d->level);
}

void UpdateButton::changeUpdaterState(bool isRunning)
{
	Q_D(UpdateButton);
	if(isRunning && d->loadingGif->state() != QMovie::Running) {
		d->loadingGif->start();
		d->ui->loaderLabel->setVisible(true);
		d->ui->statusLabel->setText(tr("Checking for updates…"));
		d->ui->statusLabel->setVisible(true);
		d->ui->checkButton->setEnabled(false);
	} else if(!isRunning && d->loadingGif->state() == QMovie::Running) {
		d->loadingGif->setPaused(true);
		d->ui->loaderLabel->setVisible(false);
		d->ui->statusLabel->setVisible(false);
		d->ui->checkButton->setEnabled(true);
	}
}

void UpdateButton::updatesReady(bool hasUpdate, bool)
{
	Q_D(UpdateButton);
	this->changeUpdaterState(false);
	if(d->showResult) {
		d->ui->checkButton->setEnabled(!hasUpdate);
		d->ui->statusLabel->setText(hasUpdate ? tr("New Update!") : tr("No new updates available"));
		d->ui->statusLabel->setVisible(true);
	}
}

void UpdateButton::controllerDestroyed()
{
	this->resetState();
	this->setDisabled(true);
	emit controllerChanged(nullptr);
}

//-----------------PRIVATE IMPLEMENTATION-----------------

QtAutoUpdater::UpdateButtonPrivate::UpdateButtonPrivate(UpdateButton *q_ptr, UpdateController *controller) :
	q_ptr(q_ptr),
	controller(nullptr),
	ui(new Ui::UpdateButton),
	level(UpdateController::ExtendedInfoLevel),
	loadingGif(new QMovie(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), QByteArray(), q_ptr)),
	showResult(true)
{
	this->ui->setupUi(this->q_ptr);
	this->loadingGif->setSpeed(200);

	this->ui->loaderLabel->setMovie(this->loadingGif);
	this->ui->loaderLabel->setVisible(false);
	this->ui->statusLabel->setVisible(false);

	QObject::connect(this->ui->checkButton, &QPushButton::clicked,
					 this->q_ptr, &UpdateButton::startUpdate);

	this->updateController(controller);
}

UpdateButtonPrivate::~UpdateButtonPrivate() {}

void UpdateButtonPrivate::updateController(UpdateController *controller)
{
	Q_Q(UpdateButton);
	this->controller = controller;
	if(controller) {
		QObject::connect(this->controller.data(), &UpdateController::runningChanged,
						 q, &UpdateButton::changeUpdaterState);
		QObject::connect(this->controller->updater(), &Updater::checkUpdatesDone,
						 q, &UpdateButton::updatesReady);
		QObject::connect(this->controller.data(), &UpdateController::destroyed,
						 q, &UpdateButton::controllerDestroyed);
	}
	q->setEnabled(controller);
}
