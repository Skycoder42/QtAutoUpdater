#ifndef UPDATEPANEL_P_H
#define UPDATEPANEL_P_H

#include <QPointer>
#include <QMovie>
#include "updatepanel.h"

namespace Ui {
	class UpdatePanel;
}

namespace QtAutoUpdater
{
	class UpdatePanelPrivate
	{
		UpdatePanel *q_ptr;
		Q_DECLARE_PUBLIC(UpdatePanel)

		QPointer<UpdateController> controller;
		Ui::UpdatePanel *ui;
		UpdateController::DisplayLevel level;
		QMovie *loadingGif;
		bool showResult;

		UpdatePanelPrivate(UpdatePanel *q_ptr, UpdateController *controller);
		~UpdatePanelPrivate();
	};
}

#endif // UPDATEPANEL_P_H
