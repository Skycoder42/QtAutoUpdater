#include "updatescheduler.h"
#include "updatescheduler_p.h"
#include <QCoreApplication>
#include <QDebug>
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
	d->settings->sync();

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

	for(UpdateSchedulerPrivate::UpdateTaskInfo info : d->updateTasks) {
		qDebug() << info.second
				 << info.first->typeIndex().hash_code()
				 << info.first->typeIndex().name()
				 << info.first->hasTasks()
				 << info.first->currentTask();
	}

	d->scheduleNextTask();
}

void UpdateScheduler::stop()
{
	Q_D(UpdateScheduler);
	if(!d->isActive)
		return;

	d->settings->remove(QStringLiteral("scheduleMemory"));
	d->settings->beginWriteArray(QStringLiteral("scheduleMemory"));
	int i = 0;
	for(UpdateSchedulerPrivate::UpdateTaskInfo info : d->updateTasks) {
		if(!info.first->hasTasks())
			continue;
		d->settings->setArrayIndex(i++);

		UpdateSchedulerPrivate::TypeInfo tInfo;
		tInfo = UpdateSchedulerPrivate::tIndexToInfo(info.first->typeIndex());
		d->settings->setValue(QStringLiteral("hash"), tInfo.first);
		d->settings->setValue(QStringLiteral("name"), tInfo.second);
		d->settings->setValue(QStringLiteral("taskID"), info.second);
		d->settings->setValue(QStringLiteral("data"), info.first->store());
	}
	d->settings->endArray();
	d->settings->sync();

	d->isActive = true;
}

void UpdateScheduler::scheduleTask(int taskGroupID, UpdateTask *task)
{
	Q_D(UpdateScheduler);
	d->updateTasks.append({task, taskGroupID});
}

int UpdateScheduler::scheduleTask(UpdateTask *task)
{
	Q_D(UpdateScheduler);

	int val;
	bool hasVal = false;
	do {
		val = (INT_MAX - RAND_MAX) + qrand();
		for(UpdateSchedulerPrivate::UpdateTaskInfo info : d->updateTasks) {
			if(info.second == val) {
				hasVal = true;
				break;
			}
		}
	} while(hasVal);

	this->scheduleTask(val, task);
	return val;
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
{
	qsrand(QDateTime::currentMSecsSinceEpoch());

	QObject::connect(qApp, &QCoreApplication::aboutToQuit,
					 this->q_ptr, &UpdateScheduler::stop);
}

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

void UpdateSchedulerPrivate::scheduleNextTask()
{
	Q_UNIMPLEMENTED();
}
