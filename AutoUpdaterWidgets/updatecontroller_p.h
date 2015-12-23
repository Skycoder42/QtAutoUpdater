#ifndef UPDATECONTROLLER_P_H
#define UPDATECONTROLLER_P_H

#include "updatecontroller.h"
#include <updater.h>
#include <QProgressDialog>

namespace QtAutoUpdater
{
	class UpdateControllerPrivate
	{
		UpdateController *q_ptr;
		Q_DECLARE_PUBLIC(UpdateController)

		QWidget *window;

		UpdateController::DisplayLevel displayLevel;
		bool running;
		Updater *mainUpdater;

		QProgressDialog *checkUpdatesProgress;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		~UpdateControllerPrivate();
	};
}

#endif // UPDATECONTROLLER_P_H
