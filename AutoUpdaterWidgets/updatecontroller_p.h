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
		bool runAdmin;
		bool adminUserEdit;
		QStringList runArgs;

		ProgressDialog *checkUpdatesProgress;
		bool wasCanceled;
		UpdateInfoDialog *infoDialog;

		QHash<int, UpdateController::DisplayLevel> updateTasks;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
		~UpdateControllerPrivate();
	};
}

#endif // UPDATECONTROLLER_P_H
