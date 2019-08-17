#include "updatebutton.h"
#include "updatebutton_p.h"
#include "ui_updatebutton.h"
#include "updatecontroller.h"

#include <QtAutoUpdaterCore/updater.h>

using namespace QtAutoUpdater;

UpdateButton::UpdateButton(QWidget *parent, UpdateController *controller) :
	QWidget{*new UpdateButtonPrivate{}, parent, {}}
{
	Q_D(UpdateButton);
	d->ui.reset(new Ui::UpdateButton{});
	d->ui->setupUi(this);

	d->loadingGif = new QMovie{QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), "gif", this};
	d->loadingGif->setSpeed(200);
	d->ui->loaderLabel->setMovie(d->loadingGif);
	d->ui->loaderLabel->setVisible(false);
	d->ui->statusLabel->setVisible(false);

	QObjectPrivate::connect(d->ui->checkButton, &QPushButton::clicked,
							d, &UpdateButtonPrivate::_q_startUpdate);

	d->updateController(controller);
}

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

void UpdateButton::setAnimationFile(const QString &animationFile, int speed)
{
	Q_D(UpdateButton);
	d->loadingGif->setFileName(animationFile);
	d->loadingGif->setSpeed(speed);
}

void UpdateButton::setAnimationDevice(QIODevice *animationDevice, int speed)
{
	Q_D(UpdateButton);
	d->loadingGif->setDevice(animationDevice);
	d->loadingGif->setSpeed(speed);
}

void UpdateButton::resetAnimationFile()
{
	Q_D(UpdateButton);
	d->loadingGif->setFileName(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"));
	d->loadingGif->setSpeed(200);
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
		resetState();
		emit controllerChanged(controller, {});
		return true;
	} else
		return false;
}

//-----------------PRIVATE IMPLEMENTATION-----------------

void UpdateButtonPrivate::updateController(UpdateController *newController)
{
	Q_Q(UpdateButton);
	controller = newController;
	if(controller) {
		connect(controller.data(), &UpdateController::runningChanged,
				this, &UpdateButtonPrivate::_q_changeUpdaterState);
		connect(controller->updater(), &Updater::checkUpdatesDone,
				this, &UpdateButtonPrivate::_q_updatesReady);
		connect(controller.data(), &UpdateController::destroyed,
				this, &UpdateButtonPrivate::_q_controllerDestroyed);
	}
	q->setEnabled(controller);
}

void UpdateButtonPrivate::_q_startUpdate()
{
	if(controller)
		controller->start(level);
}

void UpdateButtonPrivate::_q_changeUpdaterState(bool isRunning)
{
	if(isRunning && loadingGif->state() != QMovie::Running) {
		loadingGif->start();
		ui->loaderLabel->setVisible(true);
		ui->statusLabel->setText(UpdateButton::tr("Checking for updates…"));
		ui->statusLabel->setVisible(true);
		ui->checkButton->setEnabled(false);
	} else if(!isRunning && loadingGif->state() == QMovie::Running) {
		loadingGif->setPaused(true);
		ui->loaderLabel->setVisible(false);
		ui->statusLabel->setVisible(false);
		ui->checkButton->setEnabled(true);
	}
}

void UpdateButtonPrivate::_q_updatesReady(Updater::State state)
{
	_q_changeUpdaterState(false);
	if(showResult) {
		switch (state) {
		case Updater::State::NewUpdates:
			ui->checkButton->setEnabled(false);
			ui->statusLabel->setText(UpdateButton::tr("New Update!"));
			break;
		case Updater::State::NoUpdates:
			ui->checkButton->setEnabled(true);
			ui->statusLabel->setText(UpdateButton::tr("No new updates."));
			break;
		case Updater::State::Error:
			ui->checkButton->setEnabled(true);
			ui->statusLabel->setText(UpdateButton::tr("Update error!"));
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}
}

void UpdateButtonPrivate::_q_controllerDestroyed()
{
	Q_Q(UpdateButton);
	q->resetState();
	q->setDisabled(true);
	emit q->controllerChanged(nullptr, {});
}

#include "moc_updatebutton.cpp"
