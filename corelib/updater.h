#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QVersionNumber>
#include <QLoggingCategory>
#include <QScopedPointer>
#include "adminauthoriser.h"
#include "qtautoupdater_global.h"

namespace QtAutoUpdater
{
	class UpdaterPrivate;
	//! The main updater. Can check for updates and run the maintenancetool as updater
	class QTAUTOUPDATERSHARED_EXPORT Updater : public QObject
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
		struct QTAUTOUPDATERSHARED_EXPORT UpdateInfo
		{
			//! The name of the component that has an update
			QString name;
			//! The new version for that compontent
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
		explicit Updater(QObject *parent = nullptr);
		//! Constructer with an explicitly set path. Can take a parent object
		explicit Updater(const QString &maintenanceToolPath, QObject *parent = nullptr);
		//! Destroyes the updater and kills the update check
		~Updater();

		//! Returns `true`, if the updater exited normally
		bool exitedNormally() const;
		//! Returns the result-code of the last update
		int errorCode() const;
		//! returns the error output of the last update
		QByteArray errorLog() const;

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
		int scheduleUpdate(int delaySeconds, bool repeated = false);
		//! Schedules an update for a specific timepoint
		int scheduleUpdate(const QDateTime &when);
		//! Cancels the update with taskId
		void cancelScheduledUpdate(int taskId);

		//! Runs the maintenancetool as updater on exit, using the given admin authorisation
		inline void runUpdaterOnExit(AdminAuthoriser *authoriser = nullptr) {
			this->runUpdaterOnExit({QStringLiteral("--updater")}, authoriser);
		}
		//! Runs the maintenancetool as updater on exit, using the given arguments and admin authorisation
		void runUpdaterOnExit(const QStringList &arguments, AdminAuthoriser *authoriser = nullptr);
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
		QScopedPointer<UpdaterPrivate> d_ptr;
		Q_DECLARE_PRIVATE(Updater)
	};
}

Q_DECLARE_METATYPE(QtAutoUpdater::Updater::UpdateInfo)
Q_DECLARE_LOGGING_CATEGORY(logQtAutoUpdater)

QDebug QTAUTOUPDATERSHARED_EXPORT &operator<<(QDebug &debug, const QtAutoUpdater::Updater::UpdateInfo &info);

#endif // UPDATER_H
