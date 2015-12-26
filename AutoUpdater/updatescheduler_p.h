#ifndef UPDATESCHEDULER_P_H
#define UPDATESCHEDULER_P_H

#include "updatescheduler.h"
#include "updatetask.h"
#include <QMap>

namespace QtAutoUpdater
{
	class UpdateSchedulerPrivate
	{
	public:
		UpdateSchedulerPrivate();
		~UpdateSchedulerPrivate();

	private:
		UpdateScheduler *q_ptr;
		Q_DECLARE_PUBLIC(UpdateScheduler)

		QMap<std::type_index, UpdateTaskBuilder*> builderMap;
	};
}

#endif // UPDATESCHEDULER_P_H
