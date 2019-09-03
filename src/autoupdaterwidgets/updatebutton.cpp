#include "updatebutton.h"
#include "updatebutton_p.h"
#include "ui_updatebutton.h"
#include "updatecontroller.h"

#include <QtAutoUpdaterCore/updater.h>

using namespace QtAutoUpdater;

UpdateButton::UpdateButton(QWidget *parent) :
	UpdateButton{nullptr, parent}
{}

UpdateButton::UpdateButton(Updater *updater, QWidget *parent) :
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

	setEnabled(false);
	setUpdater(updater);
}

QString UpdateButton::animationFile() const
{
	Q_D(const UpdateButton);
	return d->loadingGif->fileName();
}

bool UpdateButton::isShowingResult() const
{
	Q_D(const UpdateButton);
	return d->showResult;
}

Updater *UpdateButton::updater() const
{
	Q_D(const UpdateButton);
	return d->updater;
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
	emit animationFileChanged(this->animationFile(), {});
}

void UpdateButton::setAnimationDevice(QIODevice *animationDevice, int speed)
{
	Q_D(UpdateButton);
	d->loadingGif->setDevice(animationDevice);
	d->loadingGif->setSpeed(speed);
	emit animationFileChanged(this->animationFile(), {});
}

void UpdateButton::resetAnimationFile()
{
	Q_D(UpdateButton);
	d->loadingGif->setFileName(QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"));
	d->loadingGif->setSpeed(200);
	emit animationFileChanged(this->animationFile(), {});
}

void UpdateButton::setShowResult(bool showResult)
{
	Q_D(UpdateButton);
	if (showResult == d->showResult)
		return;

	d->showResult = showResult;
	emit showResultChanged(d->showResult, {});
}

void UpdateButton::setUpdater(Updater *updater)
{
	Q_D(UpdateButton);
	if (d->updater == updater)
		return;

	if (d->updater) {
		d->updater->disconnect(this);
		if (d->updater->parent() == this)
			d->updater->deleteLater();
	}

	d->updater = updater;
	if(d->updater) {
		QObjectPrivate::connect(d->updater, &Updater::stateChanged,
								d, &UpdateButtonPrivate::_q_changeUpdaterState);
		QObjectPrivate::connect(d->updater, &Updater::destroyed,
								d, &UpdateButtonPrivate::_q_updaterDestroyed);
		connect(d->ui->checkButton, &QPushButton::clicked,
				d->updater, &Updater::checkForUpdates);
		d->_q_changeUpdaterState(d->updater->state());
	}
	setEnabled(updater);
	emit updaterChanged(d->updater, {});
}

//-----------------PRIVATE IMPLEMENTATION-----------------

void UpdateButtonPrivate::_q_changeUpdaterState(Updater::State state)
{
	if(updater->isRunning() && loadingGif->state() != QMovie::Running) {
		loadingGif->start();
		ui->loaderLabel->setVisible(true);
		ui->statusLabel->setVisible(true);
		ui->checkButton->setEnabled(false);
	} else if(!updater->isRunning() && loadingGif->state() == QMovie::Running) {
		loadingGif->setPaused(true);
		ui->loaderLabel->setVisible(false);
		ui->statusLabel->setVisible(false);
		ui->checkButton->setEnabled(true);
	}

	switch (state) {
	case Updater::State::NoUpdates:
		ui->statusLabel->setText(UpdateButton::tr("No new updates"));
		break;
	case Updater::State::Checking:
		ui->statusLabel->setText(UpdateButton::tr("Checking for updates…"));
		break;
	case Updater::State::NewUpdates:
		ui->statusLabel->setText(UpdateButton::tr("New update!"));
		break;
	case Updater::State::Error:
		ui->statusLabel->setText(UpdateButton::tr("Update error!"));
		break;
	case Updater::State::Installing:
		ui->statusLabel->setText(UpdateButton::tr("Installing updates…"));
		break;
	}
}

void UpdateButtonPrivate::_q_updaterDestroyed()
{
	Q_Q(UpdateButton);
	q->resetState();
	q->setDisabled(true);
	emit q->updaterChanged(nullptr, {});
}

#include "moc_updatebutton.cpp"
