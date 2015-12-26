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
	UpdateSchedulerPrivate::TypeInfo tInfo = UpdateSchedulerPrivate::tIndexToInfo(type);

	delete d->builderMap.take(tInfo);
	d->builderMap.insert(tInfo, builder);
}

bool UpdateScheduler::setSettingsGroup(const QString &group)
{
	Q_D(UpdateScheduler);
	if(d->isActive)
		return false;

	if(d->settings)
		d->settings->deleteLater();
	d->settings = new QSettings();
	d->settings->beginGroup(group);
	return true;
}

bool UpdateScheduler::setSettingsObject(QSettings *settingsObject)
{
	Q_D(UpdateScheduler);
	if(d->isActive || !settingsObject)
		return false;

	if(d->settings)
		d->settings->deleteLater();
	d->settings = settingsObject;
	d->settings->setParent(this);
	return true;
}

void UpdateScheduler::start()
{
	Q_D(UpdateScheduler);
	if(d->isActive)
		return;
	d->isActive = true;

	if(!d->settings) {
		d->settings = new QSettings();
		d->settings->beginGroup(QStringLiteral("QtAutoUpdater/UpdateScheduler"));
	}

	int max = d->settings->beginReadArray(QStringLiteral("scheduleMemory"));
	for(int i = 0; i < max; ++i) {
		d->settings->setArrayIndex(i);

		UpdateSchedulerPrivate::TypeInfo info;
		info.first = d->settings->value(QStringLiteral("hash")).toULongLong();
		info.second = d->settings->value(QStringLiteral("name")).toString();
		UpdateTaskBuilder *builder = d->builderMap.value(info, NULL);
		if(builder) {
			UpdateSchedulerPrivate::UpdateTaskInfo taskInfo;
			taskInfo.first = builder->buildTask(d->settings->value(QStringLiteral("data")).toByteArray());
			if(taskInfo.first) {
				taskInfo.second = d->settings->value(QStringLiteral("taskID")).toInt();
				d->updateTasks.append(taskInfo);
			}
		}
	}
	d->settings->endArray();

	d->scheduleNextTask();
}

UpdateScheduler::UpdateScheduler(UpdateSchedulerPrivate *d_ptr) :
	QObject(NULL),
	d_ptr(d_ptr)
{}

// ------------ PRIVATE IMPLEMENTATION ------------

UpdateSchedulerPrivate::UpdateSchedulerPrivate() :
	q_ptr(new UpdateScheduler(this)),
	isActive(false),
	settings(NULL),
	builderMap()
{}

UpdateSchedulerPrivate::~UpdateSchedulerPrivate()
{
	qDeleteAll(this->builderMap.values());
	delete this->q_ptr;
}

UpdateSchedulerPrivate::TypeInfo UpdateSchedulerPrivate::tIndexToInfo(const std::type_index &info)
{
	TypeInfo tInfo;
	tInfo.first = info.hash_code();
	tInfo.second = QString::fromLocal8Bit(info.name());
	return tInfo;
}

UpdateTask *UpdateSchedulerPrivate::buildTask(const TypeInfo &info, const QByteArray &data)
{
	UpdateTaskBuilder *builder = privateInstance->builderMap.value(info, NULL);
	if(builder)
		return builder->buildTask(data);
	else
		return NULL;
}
