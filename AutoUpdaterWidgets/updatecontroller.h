#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QObject>
#include <QAction>
#include <QStringList>
#include <updatetask.h>

namespace QtAutoUpdater
{
	class Updater;
	class UpdateControllerPrivate;
	class UpdateController : public QObject
	{
		Q_OBJECT

		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath CONSTANT)
		Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
		Q_PROPERTY(bool runAsAdmin READ runAsAdmin WRITE setRunAsAdmin)
		Q_PROPERTY(QStringList updateRunArgs READ updateRunArgs WRITE setUpdateRunArgs RESET resetUpdateRunArgs)

	public:
		enum DisplayLevel {
			AutomaticLevel = 0,
			ExitLevel = 1,
			InfoLevel = 2,
			ExtendedInfoLevel = 3,
			ProgressLevel = 4,
			AskLevel = 5
		};
		Q_ENUM(DisplayLevel)

		explicit UpdateController(QObject *parent = NULL);
		explicit UpdateController(QWidget *parentWindow);
		explicit UpdateController(const QString &maintenanceToolPath, QObject *parent = NULL);
		explicit UpdateController(const QString &maintenanceToolPath, QWidget *parentWindow);
		~UpdateController();

		QAction *getUpdateAction() const;
		QWidget *createUpdatePanel(QWidget *parentWidget);

		QString maintenanceToolPath() const;
		DisplayLevel currentDisplayLevel() const;
		bool isRunning() const;
		bool runAsAdmin() const;
		void setRunAsAdmin(bool runAsAdmin, bool userEditable = true);
		QStringList updateRunArgs() const;
		void setUpdateRunArgs(QStringList updateRunArgs);
		void resetUpdateRunArgs();

		Updater *getUpdater() const;

	public slots:
		bool start(DisplayLevel displayLevel = ProgressLevel);
		bool cancelUpdate(int maxDelay = 3000);

		inline int scheduleUpdate(qint64 delayMinutes, bool repeated = false, DisplayLevel displayLevel = InfoLevel) {
			return this->scheduleUpdate(new BasicLoopUpdateTask(TimeSpan(delayMinutes, TimeSpan::Minutes), repeated ? -1 : 1), displayLevel);
		}
		inline int scheduleUpdate(const QDateTime &when, DisplayLevel displayLevel = InfoLevel) {
			return this->scheduleUpdate(new TimePointUpdateTask(when), displayLevel);
		}
		int scheduleUpdate(UpdateTask *task, DisplayLevel displayLevel = InfoLevel);
		void cancelScheduledUpdate(int taskId);

	signals:
		void runningChanged(bool running);

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
