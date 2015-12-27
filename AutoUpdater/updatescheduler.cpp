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
			UpdateTask *task = builder->buildTask(d->settings->value(QStringLiteral("data")).toByteArray());
			int groupID = d->settings->value(QStringLiteral("taskID")).toInt();
			if(task && groupID)
				d->updateTasks.insert(groupID, task);
		}
	}
	d->settings->endArray();

	d->taskTimer = TimerObject::createTimer(this);
	QObject::connect(d->taskTimer, &TimerObject::taskFired,
					 this, &UpdateScheduler::taskFired,
					 Qt::QueuedConnection);
	QObject::connect(d->taskTimer, &TimerObject::taskDone,
					 this, &UpdateScheduler::taskDone,
					 Qt::QueuedConnection);

	for(UpdateTask *task : d->updateTasks) {
		QMetaObject::invokeMethod(d->taskTimer, "addTask", Qt::QueuedConnection,
								  Q_ARG(QtAutoUpdater::UpdateTask*, task));
	}
}

void UpdateScheduler::stop()
{
	Q_D(UpdateScheduler);
	if(!d->isActive)
		return;

	d->taskTimer->destroyTimer();
	d->taskTimer = NULL;

	d->settings->remove(QStringLiteral("scheduleMemory"));
	d->settings->beginWriteArray(QStringLiteral("scheduleMemory"));
	int i = 0;
	for(int groupID : d->updateTasks.keys()) {
		for(UpdateTask *task : d->updateTasks.values(groupID)) {
			if(!task->hasTasks())
				continue;
			d->settings->setArrayIndex(i++);

			UpdateSchedulerPrivate::TypeInfo tInfo;
			tInfo = UpdateSchedulerPrivate::tIndexToInfo(task->typeIndex());
			d->settings->setValue(QStringLiteral("hash"), tInfo.first);
			d->settings->setValue(QStringLiteral("name"), tInfo.second);
			d->settings->setValue(QStringLiteral("taskID"), groupID);
			d->settings->setValue(QStringLiteral("data"), task->store());
		}
	}
	d->settings->endArray();
	d->settings->sync();

	d->isActive = true;
}

void UpdateScheduler::scheduleTask(int taskGroupID, UpdateTask *task)
{
	Q_D(UpdateScheduler);
	if(taskGroupID != 0) {
		d->updateTasks.insert(taskGroupID, task);
		if(d->isActive) {
			QMetaObject::invokeMethod(d->taskTimer, "addTask", Qt::QueuedConnection,
									  Q_ARG(QtAutoUpdater::UpdateTask*, task));
		}
	} else
		delete task;
}

int UpdateScheduler::scheduleTask(UpdateTask *task)
{
	Q_D(UpdateScheduler);

	int val;
	do {
		val = (INT_MAX - RAND_MAX) + qrand();
	} while(d->updateTasks.contains(val));

	this->scheduleTask(val, task);
	return val;
}

void UpdateScheduler::cancelTaskGroup(int taskGroupID)
{
	Q_D(UpdateScheduler);
	for(UpdateTask *task : d->updateTasks.values(taskGroupID)) {
		QMetaObject::invokeMethod(d->taskTimer, "removeTask", Qt::QueuedConnection,
								  Q_ARG(QtAutoUpdater::UpdateTask*, task));
	}
}

void UpdateScheduler::taskFired(UpdateTask *task)
{
	Q_D(UpdateScheduler);
	int groupID = d->updateTasks.key(task, 0);
	if(groupID)
		emit taskReady(groupID);
}

void UpdateScheduler::taskDone(UpdateTask *task)
{
	Q_D(UpdateScheduler);
	int groupID = d->updateTasks.key(task);
	if(groupID) {
		d->updateTasks.remove(groupID, task);
		if(!d->updateTasks.contains(groupID))
			emit taskGroupFinished(groupID);
	}
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
	builderMap(),
	updateTasks(),
	taskTimer(NULL)
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
