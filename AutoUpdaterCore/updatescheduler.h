#ifndef UPDATESCHEDULER_H
#define UPDATESCHEDULER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QSettings>
#include <QMap>
#include <QMultiHash>
#include <QMutex>
#include <typeindex>
#include "updatetask.h"
#include "timerobject.h"
#include "updateschedulercontroller.h"

namespace QtAutoUpdater
{
	class UpdateScheduler : public QObject
	{
		Q_OBJECT

	public:
		static QString tIndexToInfo(const std::type_index &info);
		static UpdateTask *buildTask(const QString &info, const QByteArray &data);

		static UpdateScheduler *instance();

		bool start(QSettings *settings = NULL);
		bool stop(bool writeSettings);
		void addBuilder(const QString &id, Internal::UpdateTaskBuilder *builder);

		int scheduleTask(QtAutoUpdater::UpdateTask *task);
		void cancelTask(int taskID);

	signals:
		void taskReady(int taskID);
		void taskFinished(int taskID);

	private slots:
		void taskFired(QtAutoUpdater::UpdateTask *task);
		void taskDone(QtAutoUpdater::UpdateTask *task);

	protected:
		UpdateScheduler();
		~UpdateScheduler();

	private:
		QMutex mutex;

		bool isActive;
		QSettings *settings;
		QHash<QString, Internal::UpdateTaskBuilder*> builderMap;

		QHash<UpdateTask*, int> updateTasks;
		TimerObject *taskTimer;
	};
}

#endif // UPDATESCHEDULER_H
