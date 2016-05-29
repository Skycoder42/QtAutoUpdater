#include "updatepanel.h"
#include "ui_updatepanel.h"
#include "updatecontroller.h"
#include "updater.h"
using namespace QtAutoUpdater;

UpdatePanel::UpdatePanel(UpdateController *controller, QWidget *parent) :
	QWidget(parent),
	controller(controller),
	ui(new Ui::UpdatePanel),
	level(UpdateController::ExtendedInfoLevel),
	loadingGif(new QMovie(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), QByteArray(), this)),
	showRes(true)
{
	connect(controller, &UpdateController::destroyed,
			this, &UpdatePanel::deleteLater);

	ui->setupUi(this);
	this->loadingGif->setSpeed(200);

	this->ui->loaderLabel->setMovie(this->loadingGif);
	this->ui->loaderLabel->setVisible(false);
	this->ui->statusLabel->setVisible(false);

	connect(this->ui->checkButton, &QPushButton::clicked,
			this, &UpdatePanel::startUpdate);
    connect(this->controller.data(), &UpdateController::runningChanged,
			this, &UpdatePanel::changeUpdaterState);
	connect(this->controller->updater(), &Updater::checkUpdatesDone,
			this, &UpdatePanel::updatesReady);
    connect(this->controller.data(), &UpdateController::destroyed,
			this, [this](){
		this->setDisabled(true);
	});
}

UpdatePanel::~UpdatePanel()
{
	delete ui;
}

QString UpdatePanel::animationFile() const
{
	return this->loadingGif->fileName();
}

bool UpdatePanel::showResult() const
{
	return this->showRes;
}

UpdateController::DisplayLevel UpdatePanel::displayLevel() const
{
	return this->level;
}

void UpdatePanel::resetState()
{
	this->loadingGif->setPaused(true);
	this->ui->loaderLabel->setVisible(false);
	this->ui->statusLabel->setVisible(false);
	this->ui->checkButton->setEnabled(true);
}

void UpdatePanel::setAnimationFile(QString animationFile)
{
	this->loadingGif->setFileName(animationFile);
	emit animationFileChanged(animationFile);
}

void UpdatePanel::setAnimationDevice(QIODevice *animationDevice)
{
	this->loadingGif->setDevice(animationDevice);
	emit animationFileChanged(QString());
}

void UpdatePanel::setShowResult(bool showResult)
{
	if (this->showRes == showResult)
		return;

	this->showRes = showResult;
	emit showResultChanged(showResult);
}

void UpdatePanel::setDisplayLevel(UpdateController::DisplayLevel displayLevel)
{
	if (this->level == displayLevel)
		return;

	this->level = displayLevel;
	emit displayLevelChanged(displayLevel);
}

void UpdatePanel::startUpdate()
{
	if(!this->controller.isNull())
		this->controller->start(this->level);
}

void UpdatePanel::changeUpdaterState(bool isRunning)
{
	if(isRunning && this->loadingGif->state() != QMovie::Running) {
		this->loadingGif->start();
		this->ui->loaderLabel->setVisible(true);
		this->ui->statusLabel->setText(tr("Checking for updates…"));
		this->ui->statusLabel->setVisible(true);
		this->ui->checkButton->setEnabled(false);
	} else if(!isRunning && this->loadingGif->state() == QMovie::Running) {
		this->loadingGif->setPaused(true);
		this->ui->loaderLabel->setVisible(false);
		this->ui->statusLabel->setVisible(false);
		this->ui->checkButton->setEnabled(true);
	}
}

void UpdatePanel::updatesReady(bool hasUpdate, bool)
{
	this->changeUpdaterState(false);
	if(this->showRes) {
		this->ui->checkButton->setEnabled(!hasUpdate);
		this->ui->statusLabel->setText(hasUpdate ? tr("New Update!") : tr("No new updates available"));
		this->ui->statusLabel->setVisible(true);
	}
}
