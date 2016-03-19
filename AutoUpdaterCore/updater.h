#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QVersionNumber>
#include <QDebug>
#include "adminauthoriser.h"
#include "updatetask.h"

namespace QtAutoUpdater
{
	class UpdaterPrivate;
	//! The main updater. Can check for updates and run the updater
	class Updater : public QObject
	{
		Q_OBJECT

		//! Holds the path of the attached maintenancetool
		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath CONSTANT FINAL)
		//! Specifies whether the updater is currently checking for updates or not
		Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
		//! Holds extended information about the last update check
		Q_PROPERTY(QList<UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)

	public:
		//! Provides information about updates for components
		struct UpdateInfo
		{
			//! The name of the component that has an update
			QString name;
			//! The new version for that compontent (Check <a href="https://doc-snapshots.qt.io/qt5-5.6/qversionnumber.html" target="_blank">Qt 5.6 Documentation snapshot</a>)
			QVersionNumber version;
			//! The update download size (in Bytes)
			quint64 size;

			//! Default Constructor
			UpdateInfo();
			//! Copy Constructor
			UpdateInfo(const UpdateInfo &other);
			//! Constructor that takes name, version and size
			UpdateInfo(QString name, QVersionNumber version, quint64 size);
		};

		//! Default constructor. Can take a parent object
		explicit Updater(QObject *parent = NULL);
		//! Constructer with an explicitly set path. Can take a parent object
		explicit Updater(const QString &maintenanceToolPath, QObject *parent = NULL);
		//! Destructor
		~Updater();

		//! Returns `true`, if the updater exited normally
		bool exitedNormally() const;
		//! Returns the result-code of the last update
		int getErrorCode() const;
		//! returns the error output of the last update
		QByteArray getErrorLog() const;

		//! Returns `true` if the maintenancetool will be started on exit
		bool willRunOnExit() const;

		//! READ-Accessor for Updater::maintenanceToolPath
		QString maintenanceToolPath() const;
		//! READ-Accessor for Updater::running
		bool isRunning() const;
		//! READ-Accessor for Updater::updateInfo
		QList<UpdateInfo> updateInfo() const;

	public slots:
		//! Starts checking for updates
		bool checkForUpdates();
		//! Aborts checking for updates
		void abortUpdateCheck(int maxDelay = 5000, bool async = false);

		//! Schedules an update after a specific delay, optionally repeated
		inline int scheduleUpdate(qint64 delayMinutes, bool repeated = false) {
			return this->scheduleUpdate(new BasicLoopUpdateTask(TimeSpan(delayMinutes, TimeSpan::Minutes), repeated ? -1 : 1));
		}
		//! Schedules an update for a specific timepoint
		inline int scheduleUpdate(const QDateTime &when) {
			return this->scheduleUpdate(new TimePointUpdateTask(when));
		}
		//! Schedules an update using an UpdateTask
		int scheduleUpdate(UpdateTask *task);
		//! Cancels the update with taskId
		void cancelScheduledUpdate(int taskId);

		//! Runs the maintenancetool as updater on exit, using the given admin authorisation
		inline void runUpdaterOnExit(AdminAuthoriser *authoriser = NULL) {
			this->runUpdaterOnExit({QStringLiteral("--updater")}, authoriser);
		}
		//! Runs the maintenancetool as updater on exit, using the given arguments and admin authorisation
		void runUpdaterOnExit(const QStringList &arguments, AdminAuthoriser *authoriser = NULL);
		//! The updater will not run the updater on exit anymore
		void cancelExitRun();

	signals:
		//! Will be emitted as soon as the updater finished checking for updates
		void checkUpdatesDone(bool hasUpdates, bool hasError = false);

		//! NOTIFY-Accessor for Updater::running
		void runningChanged(bool running);
		//! NOTIFY-Accessor for Updater::updateInfo
		void updateInfoChanged(QList<QtAutoUpdater::Updater::UpdateInfo> updateInfo);

	private:
		UpdaterPrivate *d_ptr;
		Q_DECLARE_PRIVATE(Updater)
	};
}

Q_DECLARE_METATYPE(QtAutoUpdater::Updater::UpdateInfo)

QDebug &operator<<(QDebug &debug, const QtAutoUpdater::Updater::UpdateInfo &info);

#endif // UPDATER_H
