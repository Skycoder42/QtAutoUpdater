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

class QSettings;

namespace QtAutoUpdater
{

class UpdaterBackend;
class UpdateInstaller;

class UpdaterPrivate;
//! The main updater. Can check for updates and run the maintenancetool as updater
class Q_AUTOUPDATERCORE_EXPORT Updater : public QObject
{
	Q_OBJECT

	//! Specifies whether the updater is currently checking for updates or not
	Q_PROPERTY(QtAutoUpdater::Updater::State state READ state NOTIFY stateChanged)
	//! Holds extended information about the last update check
	Q_PROPERTY(QList<UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)
	Q_PROPERTY(bool runOnExit READ willRunOnExit NOTIFY runOnExitChanged)

public:
	enum class State {
		NoUpdates = 0,
		Checking,
		NewUpdates,
		Installing,

		Error = -1
	};
	Q_ENUM(State)

	static Updater *createUpdater(QObject *parent = nullptr,
								  AdminAuthoriser *authoriser = nullptr);
	static Updater *createUpdater(const QString &configPath,
								  QObject *parent = nullptr,
								  AdminAuthoriser *authoriser = nullptr);
	static Updater *createUpdater(QSettings *config,
								  QObject *parent = nullptr,
								  AdminAuthoriser *authoriser = nullptr);
	static Updater *createUpdater(QString key,
								  QVariantMap arguments,
								  QObject *parent = nullptr,
								  AdminAuthoriser *authoriser = nullptr);

	//! Destroyes the updater and kills the update check (if running)
	~Updater() override;

	Q_INVOKABLE UpdaterBackend *backend() const;


	//! readAcFn{Updater::state}
	State state() const;
	Q_INVOKABLE bool isRunning() const;
	//! readAcFn{Updater::updateInfo}
	QList<UpdateInfo> updateInfo() const;
	//! readAcFn{Updater::runOnExit}
	bool willRunOnExit() const;

	//! Schedules an update after a specific delay, optionally repeated
	Q_INVOKABLE int scheduleUpdate(int delaySeconds, bool repeated = false);
	template <typename TRep, typename TPeriod>
	int scheduleUpdate(const std::chrono::duration<TRep, TPeriod> &delay, bool repeated = false);
	//! Schedules an update for a specific timepoint
	Q_INVOKABLE int scheduleUpdate(const QDateTime &when);
	template <typename TClock, typename TDur>
	int scheduleUpdate(const std::chrono::time_point<TClock, TDur> &when);

	Q_INVOKABLE bool runUpdater(bool forceOnExit = false);

public Q_SLOTS:
	//! Starts checking for updates
	void checkForUpdates();
	//! Aborts checking for updates
	void abortUpdateCheck(int killDelay = 5000);

	//! Cancels the scheduled update with taskId
	void cancelScheduledUpdate(int taskId);

	//! The updater will not run the maintenancetool on exit anymore
	void cancelExitRun();

Q_SIGNALS:
	//! Will be emitted as soon as the updater finished checking for updates
	void checkUpdatesDone(QtAutoUpdater::Updater::State result, QPrivateSignal);
	void progressChanged(double progress, const QString &status, QPrivateSignal);
	void showInstaller(UpdateInstaller *installer, QPrivateSignal);
	void installDone(bool success, QPrivateSignal);

	//! notifyAcFn{Updater::state}
	void stateChanged(QtAutoUpdater::Updater::State state, QPrivateSignal);
	//! notifyAcFn{Updater::updateInfo}
	void updateInfoChanged(QList<QtAutoUpdater::UpdateInfo> updateInfo, QPrivateSignal);
	//! notifyAcFn{Updater::runOnExit}
	void runOnExitChanged(bool runOnExit, QPrivateSignal);

protected:
	explicit Updater(QObject *parent = nullptr);
	explicit Updater(UpdaterPrivate &dd, QObject *parent = nullptr);

private:
	Q_DECLARE_PRIVATE(Updater)
	Q_DISABLE_COPY(Updater)

	Q_PRIVATE_SLOT(d_func(), void _q_appAboutToExit())
	Q_PRIVATE_SLOT(d_func(), void _q_checkDone(bool, QList<UpdateInfo>))
	Q_PRIVATE_SLOT(d_func(), void _q_triggerInstallDone(bool))
};

template<typename TRep, typename TPeriod>
int Updater::scheduleUpdate(const std::chrono::duration<TRep, TPeriod> &delay, bool repeated)
{
	using namespace std::chrono;
	return scheduleUpdate(duration_cast<seconds>(delay), repeated);
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

#endif // QTAUTOUPDATER_UPDATER_H
