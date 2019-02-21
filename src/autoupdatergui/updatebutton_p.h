#ifndef QTAUTOUPDATER_UPDATEBUTTON_P_H
#define QTAUTOUPDATER_UPDATEBUTTON_P_H

#include "qtautoupdatergui_global.h"
#include "updatebutton.h"

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>

#include <QtGui/QMovie>

namespace Ui {
class UpdateButton;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERGUI_EXPORT UpdateButtonPrivate
{
	Q_DISABLE_COPY(UpdateButtonPrivate)
public:
	UpdateButton *q;

	QPointer<UpdateController> controller;
	QScopedPointer<Ui::UpdateButton> ui;
	UpdateController::DisplayLevel level = UpdateController::ExtendedInfoLevel;
	QMovie *loadingGif;
	bool showResult = true;

	UpdateButtonPrivate(UpdateButton *q_ptr, UpdateController *controller);
	~UpdateButtonPrivate();

	void updateController(UpdateController *newController);
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_P_H
