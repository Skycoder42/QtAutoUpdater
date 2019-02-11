#ifndef QTAUTOUPDATER_UPDATECONTROLLER_P_H
#define QTAUTOUPDATER_UPDATECONTROLLER_P_H

#include "qtautoupdatergui_global.h"
#include "updatecontroller.h"
#include "updateinfodialog_p.h"
#include "progressdialog_p.h"

#include <QtAutoUpdaterCore/updater.h>
#include <QtAutoUpdaterCore/private/simplescheduler_p.h>

#include <QtCore/QPointer>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERGUI_EXPORT UpdateControllerPrivate
{
	Q_DISABLE_COPY(UpdateControllerPrivate)
public:
	using UpdateTask = QPair<UpdateController::DisplayLevel, bool>;

	static QIcon getUpdatesIcon();

	UpdateController *q;

	QPointer<QWidget> window;

	UpdateController::DisplayLevel displayLevel = UpdateController::InfoLevel;
	bool running = false;
	Updater *mainUpdater;
	bool runAdmin = true;
	bool adminUserEdit = true;
	QStringList runArgs {QStringLiteral("--updater")};
	bool detailedInfo = true;

	QPointer<ProgressDialog> checkUpdatesProgress;
	bool wasCanceled = false;

	SimpleScheduler *scheduler;

	UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
	UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
	~UpdateControllerPrivate();
};

}

#endif // QTAUTOUPDATER_UPDATECONTROLLER_P_H
