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
    QObjectPrivate::connect(d->ui->checkButton, &QPushButton::clicked,
                            d, &UpdateButtonPrivate::_q_clicked);

    d->loadingGif = new QMovie{QStringLiteral(":/QtAutoUpdater/icons/updateRunning.gif"), "gif", this};
    d->loadingGif->setSpeed(200);
    d->loadingGif->setCacheMode(QMovie::CacheAll);
    d->ui->loaderLabel->setMovie(d->loadingGif);
    d->ui->loaderLabel->setVisible(false);
    d->ui->statusLabel->setVisible(false);

    setEnabled(false);
    setUpdater(updater);
}

Updater *UpdateButton::updater() const
{
    Q_D(const UpdateButton);
    return d->updater;
}

UpdateButton::Mode UpdateButton::mode() const
{
    Q_D(const UpdateButton);
    return d->mode;
}

QString UpdateButton::animationFile() const
{
    Q_D(const UpdateButton);
    return d->loadingGif->fileName();
}

Updater::InstallMode UpdateButton::installMode() const
{
    Q_D(const UpdateButton);
    return d->installMode;
}

Updater::InstallScope UpdateButton::installScope() const
{
    Q_D(const UpdateButton);
    return d->installScope;
}

void UpdateButton::setUpdater(Updater *updater)
{
    Q_D(UpdateButton);
    if (d->updater == updater)
        return;

    if (d->updater) {
        d->updater->disconnect(this);
        d->resetState();
        if (d->updater->parent() == this)
            d->updater->deleteLater();
    }

    d->updater = updater;
    if(d->updater) {
        QObjectPrivate::connect(d->updater, &Updater::stateChanged,
                                d, &UpdateButtonPrivate::_q_changeUpdaterState);
        QObjectPrivate::connect(d->updater, &Updater::progressChanged,
                                d, &UpdateButtonPrivate::_q_updateStatus);
        QObjectPrivate::connect(d->updater, &Updater::destroyed,
                                d, &UpdateButtonPrivate::_q_updaterDestroyed);
        d->_q_changeUpdaterState(d->updater->state());
    }
    setEnabled(updater);
    emit updaterChanged(d->updater, {});
}

void UpdateButton::setMode(Mode mode)
{
    Q_D(UpdateButton);
    if (d->mode == mode)
        return;

    d->mode = mode;
    emit modeChanged(d->mode, {});
    d->_q_changeUpdaterState(d->updater->state());
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

void UpdateButton::setInstallMode(Updater::InstallMode installMode)
{
    Q_D(UpdateButton);
    if (d->installMode == installMode)
        return;

    d->installMode = installMode;
    emit installModeChanged(d->installMode, {});
}

void UpdateButton::setInstallScope(Updater::InstallScope installScope)
{
    Q_D(UpdateButton);
    if (d->installScope == installScope)
        return;

    d->installScope = installScope;
    emit installScopeChanged(d->installScope, {});
}

//-----------------PRIVATE IMPLEMENTATION-----------------

void UpdateButtonPrivate::resetState()
{
    loadingGif->setPaused(true);
    ui->loaderLabel->setVisible(false);
    ui->statusLabel->setVisible(false);
    ui->checkButton->setText(UpdateButton::tr("Check for updates"));
    ui->checkButton->setEnabled(true);
}

void UpdateButtonPrivate::_q_changeUpdaterState(Updater::State state)
{
    bool running;
    switch (state) {
    case Updater::State::NoUpdates:
        running = false;
        ui->checkButton->setText(UpdateButton::tr("Check for updates"));
        ui->checkButton->setEnabled(true);
        ui->statusLabel->setText(UpdateButton::tr("No new updates"));
        ui->statusLabel->setVisible(mode.testFlag(ModeFlag::ShowResult));
        break;
    case Updater::State::Checking:
        running = true;
        if (mode.testFlag(ModeFlag::AllowCancel)) {
            ui->checkButton->setText(UpdateButton::tr("Cancel"));
            ui->checkButton->setEnabled(true);
        } else {
            ui->checkButton->setText(UpdateButton::tr("Check for updates"));
            ui->checkButton->setEnabled(false);
        }
        ui->statusLabel->setText(UpdateButton::tr("Checking for updates…"));
        ui->statusLabel->setVisible(true);
        break;
    case Updater::State::Canceling:
        running = true;
        ui->checkButton->setEnabled(false);
        ui->statusLabel->setText(UpdateButton::tr("Canceling…"));
        ui->statusLabel->setVisible(true);
        break;
    case Updater::State::NewUpdates:
        running = false;
        if (mode.testFlag(ModeFlag::AllowInstall))
            ui->checkButton->setText(UpdateButton::tr("Install updates"));
        else
            ui->checkButton->setText(UpdateButton::tr("Check for updates"));
        ui->checkButton->setEnabled(true);
        ui->statusLabel->setText(UpdateButton::tr("New updates are available!"));
        ui->statusLabel->setVisible(mode.testFlag(ModeFlag::ShowResult));
        break;
    case Updater::State::Error:
        running = false;
        ui->checkButton->setText(UpdateButton::tr("Check for updates"));
        ui->checkButton->setEnabled(true);
        ui->statusLabel->setText(UpdateButton::tr("Update error!"));
        ui->statusLabel->setVisible(mode.testFlag(ModeFlag::ShowResult));
        break;
    case Updater::State::Installing:
        running = true;
        ui->checkButton->setEnabled(false); // keep whatever text is there...
        ui->statusLabel->setText(UpdateButton::tr("Installing updates…"));
        ui->statusLabel->setVisible(true);
        break;
    }

    if(running && loadingGif->state() != QMovie::Running) {
        loadingGif->start();
        ui->loaderLabel->setVisible(true);
    } else if(!running && loadingGif->state() == QMovie::Running) {
        loadingGif->setPaused(true);
        ui->loaderLabel->setVisible(false);
    }
}

void UpdateButtonPrivate::_q_updateStatus(double progress, const QString &status)
{
    Q_UNUSED(progress)
    if (mode.testFlag(ModeFlag::ShowStatus) &&
        updater->state() == Updater::State::Checking)
        ui->statusLabel->setText(status.isEmpty() ?
                                     UpdateButton::tr("Checking for updates…") :
                                     status);
}

void UpdateButtonPrivate::_q_updaterDestroyed()
{
    Q_Q(UpdateButton);
    resetState();
    q->setDisabled(true);
    emit q->updaterChanged(nullptr, {});
}

void UpdateButtonPrivate::_q_clicked()
{
    if (updater) {
        switch (updater->state()) {
        case Updater::State::NewUpdates:
            if (mode.testFlag(ModeFlag::AllowInstall)) {
                updater->runUpdater(installMode, installScope);
                break;
            } else
                Q_FALLTHROUGH();
        case Updater::State::NoUpdates:
        case Updater::State::Error:
            updater->checkForUpdates();
            break;
        case Updater::State::Checking:
            updater->abortUpdateCheck();
            break;
        default:
            break;
        }
    }
}

#include "moc_updatebutton.cpp"
