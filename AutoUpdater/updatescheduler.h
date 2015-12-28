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
	class UpdateScheduler;

	// internal
	namespace Internal
	{
		class UpdateTaskBuilder
		{
		public:
			virtual inline ~UpdateTaskBuilder() {}
			virtual UpdateTask *buildTask(const QByteArray &data) = 0;
		};

		template <class Task>
		class GenericUpdateTaskBuilder : public UpdateTaskBuilder
		{
			friend class QtAutoUpdater::UpdateScheduler;
		public:
			inline UpdateTask *buildTask(const QByteArray &data) Q_DECL_OVERRIDE {
				return new Task(data);
			}
		private:
			inline GenericUpdateTaskBuilder() {}
		};
	}

	class UpdateSchedulerPrivate;
	class UpdateScheduler : public QObject
	{
		Q_OBJECT
	public:
		static UpdateScheduler *instance();

		template <class Task>
		inline void registerTaskBuilder() {
			this->registerTaskBuilder(typeid(Task), new Internal::GenericUpdateTaskBuilder<Task>());
		}

		bool setSettingsGroup(const QString &group);
		bool setSettingsObject(QSettings *settingsObject);

	public slots:
		void start();
		void stop();

		void scheduleTask(int taskGroupID, QtAutoUpdater::UpdateTask *task);
		int scheduleTask(QtAutoUpdater::UpdateTask *task);

		void cancelTaskGroup(int taskGroupID);

	signals:
		void taskReady(int taskGroupID);
		void taskGroupFinished(int taskGroupID);

	private slots:
		void taskFired(QtAutoUpdater::UpdateTask *task);
		void taskDone(QtAutoUpdater::UpdateTask *task);

	private:
		UpdateScheduler(UpdateSchedulerPrivate *d_ptr);
		void registerTaskBuilder(const std::type_index &type, Internal::UpdateTaskBuilder *builder);

		UpdateSchedulerPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateScheduler)
	};
}

#endif // UPDATESCHEDULER_H
