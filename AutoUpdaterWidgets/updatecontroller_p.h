#ifndef UPDATECONTROLLER_P_H
#define UPDATECONTROLLER_P_H

#include "updatecontroller.h"
#include "progressdialog.h"
#include "updateinfodialog.h"
#include <updater.h>

namespace QtAutoUpdater
{
	class UpdateControllerPrivate
	{
		UpdateController *q_ptr;
		Q_DECLARE_PUBLIC(UpdateController)

		QWidget *window;
		QAction *updateAction;

		UpdateController::DisplayLevel displayLevel;
		bool running;
		Updater *mainUpdater;

		ProgressDialog *checkUpdatesProgress;
		bool wasCanceled;
		UpdateInfoDialog *infoDialog;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
		~UpdateControllerPrivate();
	};
}

#endif // UPDATECONTROLLER_P_H
