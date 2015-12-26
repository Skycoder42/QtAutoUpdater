#ifndef UPDATESCHEDULER_H
#define UPDATESCHEDULER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QSettings>
#include <typeindex>
#include "updatetask.h"

namespace QtAutoUpdater
{
	template <class Task>
	class GenericUpdateTaskBuilder : public UpdateTaskBuilder
	{
		friend class UpdateScheduler;
	public:
		inline UpdateTask *buildTask(const QByteArray &data) Q_DECL_OVERRIDE {
			return new Task(data);
		}
	private:
		inline GenericUpdateTaskBuilder() {}
	};

	class UpdateSchedulerPrivate;
	class UpdateScheduler : public QObject
	{
		Q_OBJECT
	public:
		static UpdateScheduler *instance();

		void registerTaskBuilder(const std::type_index &type, UpdateTaskBuilder *builder);
		template <class Task>
		inline void registerTaskBuilder() {
			this->registerTaskBuilder(typeid(Task), new GenericUpdateTaskBuilder<Task>());
		}

		bool setSettingsGroup(const QString &group);
		bool setSettingsObject(QSettings *settingsObject);

		void start();

	public slots:

	signals:

	private:
		UpdateScheduler(UpdateSchedulerPrivate *d_ptr);

		UpdateSchedulerPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateScheduler)
	};
}

#endif // UPDATESCHEDULER_H
