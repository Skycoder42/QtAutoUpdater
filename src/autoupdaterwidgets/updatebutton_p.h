#ifndef QTAUTOUPDATER_UPDATEBUTTON_P_H
#define QTAUTOUPDATER_UPDATEBUTTON_P_H

#include "qtautoupdaterwidgets_global.h"
#include "updatebutton.h"

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>

#include <QtGui/QMovie>

#include <QtWidgets/private/qwidget_p.h>

namespace Ui {
class UpdateButton;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT UpdateButtonPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(UpdateButton)

public:
    using Mode = UpdateButton::Mode;
    using ModeFlag = UpdateButton::ModeFlag;

    QPointer<Updater> updater;
    QScopedPointer<Ui::UpdateButton> ui;
    Mode mode = ModeFlag::AllowAndShowAll;
    QMovie *loadingGif = nullptr;
    Updater::InstallMode installMode = Updater::InstallModeFlag::Parallel;
    Updater::InstallScope installScope = Updater::InstallScope::PreferInternal;

    void resetState();

    void _q_changeUpdaterState(Updater::State state);
    void _q_updateStatus(double progress, const QString &status);
    void _q_updaterDestroyed();
    void _q_clicked();
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_P_H
