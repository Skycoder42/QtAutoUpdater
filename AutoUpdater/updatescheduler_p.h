#ifndef UPDATESCHEDULER_P_H
#define UPDATESCHEDULER_P_H

#include "updatescheduler.h"

namespace QtAutoUpdater
{
	class UpdateSchedulerPrivate
	{
	public:
		UpdateSchedulerPrivate();

	private:
		UpdateScheduler *q_ptr;
		Q_DECLARE_PUBLIC(UpdateScheduler)
	};
}

#endif // UPDATESCHEDULER_P_H
