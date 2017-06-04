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
public:
	UpdateButton *q;

	QPointer<UpdateController> controller;
	QScopedPointer<Ui::UpdateButton> ui;
	UpdateController::DisplayLevel level;
	QMovie *loadingGif;
	bool showResult;

	UpdateButtonPrivate(UpdateButton *q_ptr, UpdateController *controller);
	~UpdateButtonPrivate();

	void updateController(UpdateController *controller);
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_P_H
