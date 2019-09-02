#ifndef QTAUTOUPDATER_UPDATECONTROLLER_P_H
#define QTAUTOUPDATER_UPDATECONTROLLER_P_H

#include "updatecontroller.h"
#include "updateinfodialog_p.h"
#include "progressdialog_p.h"

#include <QtAutoUpdaterCore/updater.h>

#include <QtCore/QPointer>

#include <QtCore/private/qobject_p.h>
#include <QtAutoUpdaterCore/private/simplescheduler_p.h>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT UpdateControllerPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdateController)

public:
	using UpdateTask = QPair<UpdateController::DisplayLevel, bool>;

	static QIcon getUpdatesIcon();

	Updater *updater = nullptr;
	QPointer<QWidget> window;

	UpdateController::DisplayLevel displayLevel = UpdateController::InfoLevel;
	bool running = false;
	QString desktopFileName;

	QPointer<ProgressDialog> checkUpdatesProgress;
	bool wasCanceled = false;

	SimpleScheduler *scheduler = nullptr;

	void _q_updaterCheckDone(Updater::State state);
	void _q_timerTriggered(const QVariant &parameter);
	void _q_showInstaller(UpdateInstaller *installer);
};

}

#endif // QTAUTOUPDATER_UPDATECONTROLLER_P_H
