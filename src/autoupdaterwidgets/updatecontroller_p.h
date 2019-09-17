#ifndef QTAUTOUPDATER_UPDATECONTROLLER_P_H
#define QTAUTOUPDATER_UPDATECONTROLLER_P_H

#include "updatecontroller.h"
#include "updateinfodialog_p.h"
#include "progressdialog_p.h"

#include <QtAutoUpdaterCore/updater.h>

#include <QtCore/QPointer>

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT UpdateControllerPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdateController)

public:
	using DisplayLevel = UpdateController::DisplayLevel;

	using UpdateTask = QPair<UpdateController::DisplayLevel, bool>;

	static QIcon getUpdatesIcon();

	QPointer<Updater> updater;

	DisplayLevel displayLevel = DisplayLevel::Info;
	Updater::InstallScope installScope = Updater::InstallScope::PreferInternal;
	QString desktopFileName;
	bool detailedUpdateInfo = false;

	QPointer<ProgressDialog> checkUpdatesProgress;
	bool wasChecking = false;
	bool wasCanceled = false;

	void _q_updaterStateChanged(Updater::State state);
	void _q_showInstaller(UpdateInstaller *installer);
	void _q_updaterDestroyed();

	void enterNoUpdatesState();
	void enterCheckingState();
	void enterCancelingState();
	void enterNewUpdatesState();
	void enterErrorState();
	void enterInstallingState();

	bool canShow(DisplayLevel level = DisplayLevel::Automatic) const;
	void hideProgress();
	bool showCanceled();
	void cleanUp();
};

Q_DECLARE_LOGGING_CATEGORY(logController)

}

#endif // QTAUTOUPDATER_UPDATECONTROLLER_P_H
