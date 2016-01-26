#ifndef UPDATECONTROLLER_P_H
#define UPDATECONTROLLER_P_H

#include <updater.h>
#include <QPointer>
#include "updatecontroller.h"
#include "progressdialog.h"
#include "updateinfodialog.h"

namespace QtAutoUpdater
{
	class UpdateControllerPrivate
	{
		UpdateController *q_ptr;
		Q_DECLARE_PUBLIC(UpdateController)

		QPointer<QWidget> window;

		UpdateController::DisplayLevel displayLevel;
		bool running;
		Updater *mainUpdater;
		bool runAdmin;
		bool adminUserEdit;
		QStringList runArgs;
		bool detailedInfo;

		QPointer<ProgressDialog> checkUpdatesProgress;
		bool wasCanceled;

		QHash<int, UpdateController::DisplayLevel> updateTasks;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
		~UpdateControllerPrivate();
	};
}

#endif // UPDATECONTROLLER_P_H
