#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QObject>
#include <QAction>
#include <QStringList>
#include <updatetask.h>

namespace QtAutoUpdater
{
	class Updater;
	//! @internal Private implementation
	class UpdateControllerPrivate;
	//! A class to show a controlled update GUI to the user
	class UpdateController : public QObject
	{
		Q_OBJECT

		//! Holds the path of the attached maintenancetool
		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath CONSTANT FINAL)
		//! Specifies whether the controller is currently active or not
		Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
		//! Specifies whether the controller should run the updater as admin or not
		Q_PROPERTY(bool runAsAdmin READ runAsAdmin WRITE setRunAsAdmin NOTIFY runAsAdminChanged)
		//! Holds the arguments to invoke the updater with
		Q_PROPERTY(QStringList updateRunArgs READ updateRunArgs WRITE setUpdateRunArgs RESET resetUpdateRunArgs)

	public:
		//! Defines the different display-levels of the dialog
		enum DisplayLevel {
			AutomaticLevel = 0,//!< The lowest level. Nothing will be displayed at all.
			ExitLevel = 1,/*!< The whole updating works completly automatically without displaying anything. Only
						   *   a notification that updates are ready to install will be shown if updates are available.
						   */
			InfoLevel = 2,//!< Will show information about updates if available, nothing otherwise.
			ExtendedInfoLevel = 3,//!< Will show information about the update result, for both cases, updates and no updates.
			ProgressLevel = 4,//!< Shows a (modal) progress dialog while checking for updates.
			AskLevel = 5//!< The highest level. Will ask the user if he wants to check for updates before actually checking.
		};
		Q_ENUM(DisplayLevel)

		//! Constructs a new controller with a parent. Will be application modal
		explicit UpdateController(QObject *parent = NULL);
		//! Constructs a new controller with a parent. Will modal to the parent window
		explicit UpdateController(QWidget *parentWidget);
		//! Constructs a new controller with an explicitly set path and a parent. Will modal to the parent window
		explicit UpdateController(const QString &maintenanceToolPath, QObject *parent = NULL);
		//! Constructs a new controller with an explicitly set path and a parent. Will be application modal
		explicit UpdateController(const QString &maintenanceToolPath, QWidget *parentWidget);
		//! Destructor
		~UpdateController();

		//! Returns an QAction to start this controller from
		QAction *getUpdateAction() const;
		//! Creates a new "UpdatePanel" widget to place in your GUI
		QWidget *createUpdatePanel(QWidget *parentWidget);

		//! READ-Accessor for UpdateController::maintenanceToolPath
		QString maintenanceToolPath() const;
		//! READ-Accessor for UpdateController::currentDisplayLevel
		DisplayLevel currentDisplayLevel() const;
		//! READ-Accessor for UpdateController::running
		bool isRunning() const;
		//! READ-Accessor for UpdateController::runAsAdmin
		bool runAsAdmin() const;
		//! WRITE-Accessor for UpdateController::runAsAdmin
		void setRunAsAdmin(bool runAsAdmin, bool userEditable = true);
		//! READ-Accessor for UpdateController::updateRunArgs
		QStringList updateRunArgs() const;
		//! WRITE-Accessor for UpdateController::updateRunArgs
		void setUpdateRunArgs(QStringList updateRunArgs);
		//! RESET-Accessor for UpdateController::updateRunArgs
		void resetUpdateRunArgs();

		//! Returns the Updater object used by the controller
		const Updater * getUpdater() const;

		//! Returns the parent window
		QWidget* parentWidget() const;
		//! Changes the updaters parent to a new widget
		void setParent(QWidget* parent);
		//! Changes the updaters parent to an object without a widget
		void setParent(QObject* parent);

	public slots:
		//! Starts the controller with the specified level.
		bool start(DisplayLevel displayLevel = InfoLevel);
		//! Tries to cancel the controllers update
		bool cancelUpdate(int maxDelay = 3000);

		//! Schedules an update after a specific delay, optionally repeated
		inline int scheduleUpdate(qint64 delayMinutes, bool repeated = false, DisplayLevel displayLevel = InfoLevel) {
			return this->scheduleUpdate(new BasicLoopUpdateTask(TimeSpan(delayMinutes, TimeSpan::Minutes), repeated ? -1 : 1), displayLevel);
		}
		//! Schedules an update for a specific timepoint
		inline int scheduleUpdate(const QDateTime &when, DisplayLevel displayLevel = InfoLevel) {
			return this->scheduleUpdate(new TimePointUpdateTask(when), displayLevel);
		}
		//! Schedules an update using an UpdateTask
		int scheduleUpdate(UpdateTask *task, DisplayLevel displayLevel = InfoLevel);
		//! Cancels the update with taskId
		void cancelScheduledUpdate(int taskId);

	signals:
		//! NOTIFY-Accessor for UpdateController::running
		void runningChanged(bool running);
		//! NOTIFY-Accessor for UpdateController::runAsAdmin
		void runAsAdminChanged(bool runAsAdmin);

	private slots:
		void checkUpdatesDone(bool hasUpdates, bool hasError);

		void taskReady(int groupID);
		void taskDone(int groupID);

	private:
		UpdateControllerPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateController)
	};
}

#endif // UPDATECONTROLLER_H
