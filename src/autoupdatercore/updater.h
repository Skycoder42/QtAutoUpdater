#ifndef QTAUTOUPDATER_UPDATER_H
#define QTAUTOUPDATER_UPDATER_H

#include <chrono>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qdatetime.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updateinfo.h"
#include "QtAutoUpdaterCore/adminauthoriser.h"
#include "QtAutoUpdaterCore/updaterbackend.h"

class QSettings;

namespace QtAutoUpdater
{

class UpdateInstaller;

class UpdaterPrivate;
//! The main updater. Can check for updates and trigger update installations
class Q_AUTOUPDATERCORE_EXPORT Updater : public QObject
{
	Q_OBJECT

	//! Holds the features of the backend used by the updater
	Q_PROPERTY(QtAutoUpdater::UpdaterBackend::Features features READ features CONSTANT)
	//! Reports the state the updater is currently in
	Q_PROPERTY(QtAutoUpdater::Updater::State state READ state NOTIFY stateChanged)
	//! Specifies whether the updater is currently running or not
	Q_PROPERTY(bool running	READ isRunning NOTIFY runningChanged)
	//! Holds extended information about the last update check
	Q_PROPERTY(QList<QtAutoUpdater::UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)
	//! Specifies whether an installer will be launched when the application exits
	Q_PROPERTY(bool runOnExit READ willRunOnExit RESET cancelExitRun NOTIFY runOnExitChanged)

public:
	//! The state of the updater
	enum class State {
		NoUpdates = 0,  //!< The updater is not active and there are no updates to be installed
		Checking,  //!< The updater is currently checking for new updates
		Canceling,  //!< The updater is trying to cancel an update check
		NewUpdates,  //!< The updater is not active, but new updates are available
		Installing,  //!< The updater is blocked because there is currently an installation running

		Error = -1  //!< The updater is not active because an error occured
	};
	Q_ENUM(State)

	//! Flags to indicate how to run the installer
	enum class InstallModeFlag {
		Parallel = 0x00,  //!< Prefer the installation to be run in parallel to the running application
		OnExit = 0x01,  //!< Prefer the installation to be run when the application exits

		Force = 0x02, //!< Force the Parallel or OnExit behaviour and fail otherwise

		ForceOnExit = (OnExit | Force) //!< Short for OnExit | Force
	};
	Q_DECLARE_FLAGS(InstallMode, InstallModeFlag)
	Q_FLAG(InstallMode)

	//! Defines the different installation scopes
	enum class InstallScope {
		PreferInternal,  //!< Prefer the installer to be run as internal window of this application
		PreferExternal  //!< Prefer the installer to be run as external tool
	};
	Q_ENUM(InstallScope)

	//! Returns a list of all currently available backends
	static QStringList supportedUpdaterBackends();

	//! Creates an updater instance using the default configuration
	static Updater *create(QObject *parent = nullptr);
	//! Creates an updater instance using the given configuration file
	static Updater *create(const QString &configPath,
						   QObject *parent = nullptr);
	//! Creates an updater instance using the given configuration object
	static Updater *create(QSettings *config,
						   QObject *parent = nullptr);
	//! Creates an updater instance using the given backend and configuration
	static Updater *create(QString key,
						   QVariantMap configuration,
						   QObject *parent = nullptr);
	//! Creates an updater instance using the given configuration reader
	static Updater *create(UpdaterBackend::IConfigReader *configReader,
						   QObject *parent = nullptr);

	//! Destroyes the updater and kills the update check (if running)
	~Updater() override;

	//! Returns the backend used by this updater instance
	Q_INVOKABLE QtAutoUpdater::UpdaterBackend *backend() const;

	//! @readAcFn{Updater::features}
	UpdaterBackend::Features features() const;
	//! @readAcFn{Updater::state}
	State state() const;
	//! @readAcFn{Updater::running}
	bool isRunning() const;
	//! @readAcFn{Updater::updateInfo}
	QList<UpdateInfo> updateInfo() const;
	//! @readAcFn{Updater::runOnExit}
	bool willRunOnExit() const;

	//! Schedules an update after a specific delay, optionally repeated
	int scheduleUpdate(std::chrono::seconds delaySeconds, bool repeated = false);
	//! @copydoc Updater::scheduleUpdate(std::chrono::seconds, bool)
	Q_INVOKABLE int scheduleUpdate(int delaySeconds, bool repeated = false);
	//! @copydoc Updater::scheduleUpdate(std::chrono::seconds, bool)
	template <typename TRep, typename TPeriod>
	int scheduleUpdate(const std::chrono::duration<TRep, TPeriod> &delaySeconds, bool repeated = false);
	//! Schedules an update for a specific timepoint
	Q_INVOKABLE int scheduleUpdate(const QDateTime &when);
	//! @copydoc Updater::scheduleUpdate(const QDateTime &)
	template <typename TClock, typename TDur>
	int scheduleUpdate(const std::chrono::time_point<TClock, TDur> &when);

	//! Launches an update installer based on the given mode and scope
	Q_INVOKABLE bool runUpdater(InstallMode mode = InstallModeFlag::Parallel,
								InstallScope scope = InstallScope::PreferInternal);

public Q_SLOTS:
	//! Starts checking for updates
	void checkForUpdates();
	//! Aborts checking for updates
	void abortUpdateCheck(int killDelay = 5000);

	//! Cancels the scheduled update with taskId
	void cancelScheduledUpdate(int taskId);

	//! @resetAcFn{runOnExit}
	void cancelExitRun();

Q_SIGNALS:
	//! Will be emitted as soon as the updater finished checking for updates
	void checkUpdatesDone(QtAutoUpdater::Updater::State result, QPrivateSignal);
	//! Will be emitted to report a change in the update check progress
	void progressChanged(double progress, const QString &status, QPrivateSignal);
	//! Will be emitted when a started installation requires an installer to be shown
	void showInstaller(QtAutoUpdater::UpdateInstaller *installer, QPrivateSignal);
	//! Will be emitted when a started parallel installation has finished
	void installDone(bool success, QPrivateSignal);

	//! @notifyAcFn{Updater::state}
	void stateChanged(QtAutoUpdater::Updater::State state, QPrivateSignal);
	//! @notifyAcFn{Updater::running}
	void runningChanged(bool running, QPrivateSignal);
	//! @notifyAcFn{Updater::updateInfo}
	void updateInfoChanged(QList<QtAutoUpdater::UpdateInfo> updateInfo, QPrivateSignal);
	//! @notifyAcFn{Updater::runOnExit}
	void runOnExitChanged(bool runOnExit, QPrivateSignal);

protected:
	//! @private
	explicit Updater(QObject *parent = nullptr);
	//! @private
	explicit Updater(UpdaterPrivate &dd, QObject *parent = nullptr);

private:
	Q_DECLARE_PRIVATE(Updater)
	Q_DISABLE_COPY(Updater)

	Q_PRIVATE_SLOT(d_func(), void _q_appAboutToExit())
	Q_PRIVATE_SLOT(d_func(), void _q_checkDone(bool, QList<UpdateInfo>))
	Q_PRIVATE_SLOT(d_func(), void _q_triggerInstallDone(bool))
};

template<typename TRep, typename TPeriod>
int Updater::scheduleUpdate(const std::chrono::duration<TRep, TPeriod> &delaySeconds, bool repeated)
{
	using namespace std::chrono;
	return scheduleUpdate(duration_cast<seconds>(delaySeconds), repeated);
}

template<typename TClock, typename TDur>
int Updater::scheduleUpdate(const std::chrono::time_point<TClock, TDur> &when)
{
	using namespace std::chrono;
	return scheduleUpdate(QDateTime::fromSecsSinceEpoch(duration_cast<seconds>(
															time_point_cast<system_clock>(when)
															.time_since_epoch())));
}

}

Q_DECLARE_METATYPE(QtAutoUpdater::Updater::State)
Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::Updater::InstallMode)

#endif // QTAUTOUPDATER_UPDATER_H
