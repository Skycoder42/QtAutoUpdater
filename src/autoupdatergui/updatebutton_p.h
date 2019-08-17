#ifndef QTAUTOUPDATER_UPDATEBUTTON_P_H
#define QTAUTOUPDATER_UPDATEBUTTON_P_H

#include "qtautoupdatergui_global.h"
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

class Q_AUTOUPDATERGUI_EXPORT UpdateButtonPrivate : public QWidgetPrivate
{
	Q_DECLARE_PUBLIC(UpdateButton)

public:
	QPointer<UpdateController> controller;
	QScopedPointer<Ui::UpdateButton> ui;
	UpdateController::DisplayLevel level = UpdateController::ExtendedInfoLevel;
	QMovie *loadingGif;
	bool showResult = true;

	void updateController(UpdateController *newController);

	void _q_startUpdate();
	void _q_changeUpdaterState(bool isRunning);
	void _q_updatesReady(Updater::State state);
	void _q_controllerDestroyed();
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_P_H
