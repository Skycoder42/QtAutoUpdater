#ifndef UPDATECONTROLLER_P_H
#define UPDATECONTROLLER_P_H

#include <updater.h>
#include <QPointer>
#include "updatecontroller.h"
#include "progressdialog.h"
#include "updateinfodialog.h"
#include "simplescheduler.h"

namespace QtAutoUpdater
{
	class UpdateControllerPrivate//TODO load icon indirectly -> easier replacement
	{
	public:
		typedef QPair<UpdateController::DisplayLevel, bool> UpdateTask;

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

		SimpleScheduler *scheduler;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
		~UpdateControllerPrivate();
	};
}

#endif // UPDATECONTROLLER_P_H
