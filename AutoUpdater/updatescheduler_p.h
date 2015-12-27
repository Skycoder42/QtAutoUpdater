#ifndef UPDATESCHEDULER_P_H
#define UPDATESCHEDULER_P_H

#include "updatescheduler.h"
#include "updatetask.h"
#include <QMap>
#include <QMultiHash>
#include "timerobject.h"

namespace QtAutoUpdater
{
	class UpdateSchedulerPrivate
	{
	public:

		UpdateSchedulerPrivate();
		~UpdateSchedulerPrivate();

		static QString tIndexToInfo(const std::type_index &info);
		static UpdateTask *buildTask(const QString &info, const QByteArray &data);

	private:
		UpdateScheduler *q_ptr;
		Q_DECLARE_PUBLIC(UpdateScheduler)

		bool isActive;
		QSettings *settings;
		QMap<QString, UpdateTaskBuilder*> builderMap;

		QMultiHash<int, UpdateTask*> updateTasks;
		TimerObject *taskTimer;
	};
}

#endif // UPDATESCHEDULER_P_H
