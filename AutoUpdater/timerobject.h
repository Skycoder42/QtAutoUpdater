#ifndef TIMEROBJECT_H
#define TIMEROBJECT_H

#include <QObject>
#include <QHash>
#include "updatetask.h"

namespace QtAutoUpdater
{
	class TimerObject : public QObject
	{
		Q_OBJECT
	public:
		static TimerObject *createTimer(QObject *threadParent);

		void destroyTimer();

	public slots:
		void addTask(QtAutoUpdater::UpdateTask *task);

	signals:
		void taskFired(QtAutoUpdater::UpdateTask *task);
		void taskDone(QtAutoUpdater::UpdateTask *task);

	protected:
		void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

	private:
		explicit TimerObject(QObject *parent = 0);

		QHash<int, UpdateTask *> taskMap;
	};
}

#endif // TIMEROBJECT_H
