#ifndef UPDATESCHEDULERCONTROLLER_H
#define UPDATESCHEDULERCONTROLLER_H

#include <QObject>
#include <QSettings>
#include <typeindex>
#include "updatetask.h"

namespace QtAutoUpdater
{
	class UpdateSchedulerController;

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
			friend class QtAutoUpdater::UpdateSchedulerController;
		public:
			inline UpdateTask *buildTask(const QByteArray &data) Q_DECL_OVERRIDE {
				return new Task(data);
			}
		private:
			inline GenericUpdateTaskBuilder() {}
		};
	}

	//! A static controller class to allow minor scheduler control
	class UpdateSchedulerController
	{
	public:
		//! Registers an UpdateTask subclass for storing
		template <class Task>
		static inline void registerStoredTask() {
			UpdateSchedulerController::addTaskBuilder(typeid(Task),
													  new Internal::GenericUpdateTaskBuilder<Task>());
		}

		//! Changes the settings group
		static void setSettingsGroup(const QString &group);
		//! Changes the settings object
		static void setSettingsObject(QSettings *settingsObject);

	private:
		static void addTaskBuilder(const std::type_index &type, Internal::UpdateTaskBuilder *builder);
	};
}

#endif // UPDATESCHEDULERCONTROLLER_H
