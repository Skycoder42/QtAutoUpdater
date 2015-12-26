#include "updatescheduler.h"
#include "updatescheduler_p.h"
#include <QCoreApplication>
using namespace QtAutoUpdater;

Q_GLOBAL_STATIC(UpdateSchedulerPrivate, privateInstance)

UpdateScheduler *UpdateScheduler::instance()
{
	return privateInstance->q_ptr;
}

void UpdateScheduler::registerTaskBuilder(const std::type_index &type, UpdateTaskBuilder *builder)
{
	Q_D(UpdateScheduler);
	delete d->builderMap.take(type);//TODO ok so???
	d->builderMap.insert(type, builder);
}

UpdateScheduler::UpdateScheduler(UpdateSchedulerPrivate *d_ptr) :
	QObject(NULL),
	d_ptr(d_ptr)
{}

// ------------ PRIVATE IMPLEMENTATION ------------

UpdateSchedulerPrivate::UpdateSchedulerPrivate() :
	q_ptr(new UpdateScheduler(this))
{}

UpdateSchedulerPrivate::~UpdateSchedulerPrivate()
{
	delete this->q_ptr;
}
