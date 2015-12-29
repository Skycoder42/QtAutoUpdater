#include "updateschedulercontroller.h"
#include "updatescheduler.h"
using namespace QtAutoUpdater;

void UpdateSchedulerController::setSettingsGroup(const QString &group)
{
	UpdateScheduler *scheduler = UpdateScheduler::instance();
	scheduler->stop(false);
	QSettings *settings = new QSettings(scheduler);
	settings->beginGroup(group);
	scheduler->start(settings);
}

void UpdateSchedulerController::setSettingsObject(QSettings *settingsObject)
{
	UpdateScheduler *scheduler = UpdateScheduler::instance();
	scheduler->stop(false);
	settingsObject->setParent(scheduler);
	scheduler->start(settingsObject);
}

void UpdateSchedulerController::registerTaskBuilder(const std::type_index &type, Internal::UpdateTaskBuilder *builder)
{
	UpdateScheduler::instance()->addBuilder(UpdateScheduler::tIndexToInfo(type),
											builder);
}
