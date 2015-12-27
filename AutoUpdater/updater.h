#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#if QT_VERSION >= 0x050600
#include <QVersionNumber>
#else
struct QVersionNumber : public QString {
	QVersionNumber(const QString &str = QString()) : QString(str) {}

	inline QString toString() const {return (*this);}
	static inline QVersionNumber fromString(const QString &string, int * = 0) {return QVersionNumber(string);}
};
#endif
#include <QDebug>
#include "adminauthoriser.h"
#include "updatetask.h"

namespace QtAutoUpdater
{
	class UpdaterPrivate;
	class Updater : public QObject
	{
		Q_OBJECT

		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath CONSTANT)
		Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
		Q_PROPERTY(QList<UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)

	public:
		struct UpdateInfo
		{
			QString name;
			QVersionNumber version;
			quint64 size;

			UpdateInfo();
			UpdateInfo(const UpdateInfo &other);
			UpdateInfo(QString name, QVersionNumber version, quint64 size);
		};

		explicit Updater(QObject *parent = NULL);
		explicit Updater(const QString &maintenanceToolPath, QObject *parent = NULL);
		~Updater();

		bool exitedNormally() const;
		int getErrorCode() const;
		QByteArray getErrorLog() const;

		bool willRunOnExit() const;

		QString maintenanceToolPath() const;
		bool isRunning() const;
		QList<UpdateInfo> updateInfo() const;

	public slots:
		bool checkForUpdates();
		void abortUpdateCheck(int maxDelay = 5000, bool async = false);

		inline int scheduleUpdate(qint64 delayMinutes, bool repeated = false) {
			return this->scheduleUpdate(new BasicLoopUpdateTask(TimeSpan(delayMinutes, TimeSpan::Minutes), repeated ? -1 : 1));
		}
		inline int scheduleUpdate(const QDateTime &when) {
			return this->scheduleUpdate(new TimePointUpdateTask(when));
		}
		int scheduleUpdate(UpdateTask *task);
		void cancelScheduledUpdate(int taskId);

		inline void runUpdaterOnExit(AdminAuthoriser *authoriser = NULL) {
			this->runUpdaterOnExit({QStringLiteral("--updater")}, authoriser);
		}
		void runUpdaterOnExit(const QStringList &arguments, AdminAuthoriser *authoriser = NULL);
		void cancelExitRun();

	signals:
		void checkUpdatesDone(bool hasUpdates, bool hasError);

		void runningChanged(bool running);
		void updateInfoChanged(QList<QtAutoUpdater::Updater::UpdateInfo> updateInfo);

	private:
		UpdaterPrivate *d_ptr;
		Q_DECLARE_PRIVATE(Updater)
	};
}

Q_DECLARE_METATYPE(QtAutoUpdater::Updater::UpdateInfo)

QDebug &operator<<(QDebug &debug, const QtAutoUpdater::Updater::UpdateInfo &info);

#endif // UPDATER_H
