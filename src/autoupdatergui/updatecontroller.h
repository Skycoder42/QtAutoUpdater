#ifndef QTAUTOUPDATER_UPDATECONTROLLER_H
#define QTAUTOUPDATER_UPDATECONTROLLER_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

#include <QtAutoUpdaterCore/updater.h>

#include <QtWidgets/qaction.h>

#include "QtAutoUpdaterGui/qtautoupdatergui_global.h"

namespace QtAutoUpdater
{

class Updater;
class UpdateControllerPrivate;
//! A class to show a controlled update GUI to the user
class Q_AUTOUPDATERGUI_EXPORT UpdateController : public QObject
{
	Q_OBJECT

	//! Holds the widget who's window should be used as parent for all dialogs
	Q_PROPERTY(QWidget* parentWindow READ parentWindow WRITE setParentWindow)
	//! Specifies whether the controller is currently active or not
	Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
	Q_PROPERTY(QString desktopFileName READ desktopFileName WRITE setDesktopFileName)

	// TODO add auto detach and reparenting of updater if run on exit is set

public:
	//! Defines the different display-levels of the dialog
	enum DisplayLevel {
		AutomaticLevel = 0,//!< The lowest level. Nothing will be displayed at all. The programm will be auto-closed.
		ExitLevel = 1,/*!< The whole updating works completly automatically without displaying anything. Only
					   *   a notification that updates are ready to install will be shown if updates are available.
					   */
		InfoLevel = 2,//!< Will show information about updates if available, nothing otherwise.
		ExtendedInfoLevel = 3,//!< Will show information about the update result, for both cases, updates and no updates.
		ProgressLevel = 4,//!< Shows a (modal) progress dialog while checking for updates.
		AskLevel = 5//!< The highest level. Will ask the user if he wants to check for updates before actually checking.
	};
	Q_ENUM(DisplayLevel)

	//! Constructs a new controller with an explicitly set path and a parent. Will be application modal
	explicit UpdateController(Updater *updater, QObject *parent = nullptr);
	//! Constructs a new controller with an explicitly set path and a parent. Will modal to the parent window
	explicit UpdateController(Updater *updater, QWidget *parentWindow, QObject *parent = nullptr);
	~UpdateController() override;

	//! Create a QAction to start this controller from
	QAction *createUpdateAction(QObject *parent);
	QAction *createUpdateAction(DisplayLevel displayLevel, QObject *parent);

	//! @readAcFn{UpdateController::parentWindow}
	QWidget* parentWindow() const;
	//! @readAcFn{UpdateController::currentDisplayLevel}
	DisplayLevel currentDisplayLevel() const;
	//! @readAcFn{UpdateController::running}
	bool isRunning() const;
	//! @readAcFn{UpdateController::desktopFileName}
	QString desktopFileName() const;

	//! Returns the Updater object used by the controller
	Updater *updater() const;

	template <typename TRep, typename TPeriod>
	int scheduleUpdate(const std::chrono::duration<TRep, TPeriod> &delay, bool repeated = false, DisplayLevel displayLevel = InfoLevel);
	template <typename TClock, typename TDur>
	int scheduleUpdate(const std::chrono::time_point<TClock, TDur> &when, DisplayLevel displayLevel = InfoLevel);

public Q_SLOTS:
	//! @writeAcFn{UpdateController::parentWindow}
	void setParentWindow(QWidget* parentWindow);
	//! @writeAcFn{UpdateController::desktopFileName}
	void setDesktopFileName(QString desktopFileName);

	//! Starts the controller with the specified display level
	bool start(DisplayLevel displayLevel = InfoLevel);
	//! Tries to cancel the controllers update
	bool cancelUpdate(int maxDelay = 3000);

	//! Schedules an update after a specific delay, optionally repeated
	int scheduleUpdate(int delaySeconds, bool repeated = false, DisplayLevel displayLevel = InfoLevel);
	//! Schedules an update for a specific timepoint
	int scheduleUpdate(const QDateTime &when, DisplayLevel displayLevel = InfoLevel);
	//! Cancels the update with taskId
	void cancelScheduledUpdate(int taskId);

Q_SIGNALS:
	//! @notifyAcFn{UpdateController::running}
	void runningChanged(bool running, QPrivateSignal);

private:
	Q_DECLARE_PRIVATE(UpdateController)
	Q_DISABLE_COPY(UpdateController)

	Q_PRIVATE_SLOT(d_func(), void _q_updaterCheckDone(Updater::State))
	Q_PRIVATE_SLOT(d_func(), void _q_timerTriggered(const QVariant &))
};

}

#endif // QTAUTOUPDATER_UPDATECONTROLLER_H
