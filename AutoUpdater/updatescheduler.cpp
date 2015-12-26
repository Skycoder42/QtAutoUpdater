#include "updatescheduler.h"
#include "updatescheduler_p.h"
#include <QCoreApplication>
using namespace QtAutoUpdater;

Q_GLOBAL_STATIC(UpdateSchedulerPrivate, privateInstance)

UpdateScheduler *UpdateScheduler::instance()
{
	return privateInstance->q_ptr;
}

UpdateScheduler::UpdateScheduler(UpdateSchedulerPrivate *d_ptr) :
	QObject(NULL),
	d_ptr(d_ptr)
{

}

UpdateSchedulerPrivate::UpdateSchedulerPrivate() :
	q_ptr(new UpdateScheduler(this))
{}
