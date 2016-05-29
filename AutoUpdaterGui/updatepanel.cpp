#include "updatepanel.h"
#include "updatepanel_p.h"
#include "ui_updatepanel.h"
#include "updatecontroller.h"
#include "updater.h"
using namespace QtAutoUpdater;

UpdatePanel::UpdatePanel(UpdateController *controller, QWidget *parent) :
	QWidget(parent),
	d_ptr(new UpdatePanelPrivate(this, controller))
{}

UpdatePanel::~UpdatePanel()
{
	delete this->d_ptr;
}

QString UpdatePanel::animationFile() const
{
	const Q_D(UpdatePanel);
	return d->loadingGif->fileName();
}

bool UpdatePanel::isShowingResult() const
{
	const Q_D(UpdatePanel);
	return d->showResult;
}

UpdateController::DisplayLevel UpdatePanel::displayLevel() const
{
	const Q_D(UpdatePanel);
	return d->level;
}

void UpdatePanel::resetState()
{
	Q_D(UpdatePanel);
	d->loadingGif->setPaused(true);
	d->ui->loaderLabel->setVisible(false);
	d->ui->statusLabel->setVisible(false);
	d->ui->checkButton->setEnabled(true);
}

void UpdatePanel::setAnimationFile(QString animationFile)
{
	Q_D(UpdatePanel);
	d->loadingGif->setFileName(animationFile);
	emit animationFileChanged(animationFile);
}

void UpdatePanel::setAnimationDevice(QIODevice *animationDevice)
{
	Q_D(UpdatePanel);
	d->loadingGif->setDevice(animationDevice);
	emit animationFileChanged(QString());
}

void UpdatePanel::resetAnimationFile()
{
	Q_D(UpdatePanel);
	d->loadingGif->setFileName(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"));
	emit animationFileChanged(d->loadingGif->fileName());
}

void UpdatePanel::setShowResult(bool showResult)
{
	Q_D(UpdatePanel);
	if (d->showResult == showResult)
		return;

	d->showResult = showResult;
	emit showResultChanged(showResult);
}

void UpdatePanel::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	Q_D(UpdatePanel);
	if (d->level == displayLevel)
		return;

	d->level = displayLevel;
	emit displayLevelChanged(displayLevel);
}

void UpdatePanel::startUpdate()
{
	Q_D(UpdatePanel);
	if(!d->controller.isNull())
		d->controller->start(d->level);
}

void UpdatePanel::changeUpdaterState(bool isRunning)
{
	Q_D(UpdatePanel);
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

void UpdatePanel::updatesReady(bool hasUpdate, bool)
{
	Q_D(UpdatePanel);
	this->changeUpdaterState(false);
	if(d->showResult) {
		d->ui->checkButton->setEnabled(!hasUpdate);
		d->ui->statusLabel->setText(hasUpdate ? tr("New Update!") : tr("No new updates available"));
		d->ui->statusLabel->setVisible(true);
	}
}

//-----------------PRIVATE IMPLEMENTATION-----------------

QtAutoUpdater::UpdatePanelPrivate::UpdatePanelPrivate(UpdatePanel *q_ptr, UpdateController *controller) :
	q_ptr(q_ptr),
	controller(controller),
	ui(new Ui::UpdatePanel),
	level(UpdateController::ExtendedInfoLevel),
	loadingGif(new QMovie(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), QByteArray(), q_ptr)),
	showResult(true)
{
	QObject::connect(controller, &UpdateController::destroyed,
					 this->q_ptr, &UpdatePanel::deleteLater);

	this->ui->setupUi(this->q_ptr);
	this->loadingGif->setSpeed(200);

	this->ui->loaderLabel->setMovie(this->loadingGif);
	this->ui->loaderLabel->setVisible(false);
	this->ui->statusLabel->setVisible(false);

	QObject::connect(this->ui->checkButton, &QPushButton::clicked,
					 this->q_ptr, &UpdatePanel::startUpdate);
	QObject::connect(this->controller.data(), &UpdateController::runningChanged,
					 this->q_ptr, &UpdatePanel::changeUpdaterState);
	QObject::connect(this->controller->updater(), &Updater::checkUpdatesDone,
					 this->q_ptr, &UpdatePanel::updatesReady);
	QObject::connect(this->controller.data(), &UpdateController::destroyed,
					 this->q_ptr, [this](){
		this->q_ptr->setDisabled(true);
	});
}

UpdatePanelPrivate::~UpdatePanelPrivate()
{
	delete this->ui;
}
