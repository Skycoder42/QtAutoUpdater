#include "updatepanel.h"
#include "ui_updatepanel.h"
#include "updatecontroller.h"
#include "updater.h"
using namespace QtAutoUpdater;

UpdatePanel::UpdatePanel(UpdateController *controller, QWidget *parent) :
	QWidget(parent),
	controller(controller),
	ui(new Ui::UpdatePanel),
	loadingGif(new QMovie(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), "gif", this))
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

void UpdatePanel::startUpdate()
{
	if(!this->controller.isNull())
		this->controller->start(UpdateController::ExtendedInfoLevel);
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
		this->ui->statusLabel->clear();
		this->ui->checkButton->setEnabled(true);
	}
}

void UpdatePanel::updatesReady(bool hasUpdate, bool)
{
	this->changeUpdaterState(false);
	this->ui->statusLabel->setText(hasUpdate ? tr("New Update!") : tr("No new updates available"));
	this->ui->checkButton->setEnabled(!hasUpdate);
}
