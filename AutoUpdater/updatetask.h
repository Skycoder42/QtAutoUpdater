#ifndef UPDATETASK_H
#define UPDATETASK_H

#include <QDateTime>
#include <QVariant>

namespace QtAutoUpdater
{
	class UpdateTask
	{
	public:
		static UpdateTask *createFromData(const QVariant &data);//HERE

		virtual bool isValid() const = 0;

		virtual bool hasTasks() const = 0;
		virtual QDateTime currentTask() const = 0;
		virtual bool nextTask() = 0;

		virtual QVariant store() const = 0;
	};
}

#endif // UPDATETASK_H
