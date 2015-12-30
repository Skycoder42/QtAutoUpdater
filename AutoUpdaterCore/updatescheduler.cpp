#include "updatescheduler.h"
#include <QCoreApplication>
#include <QDebug>
using namespace QtAutoUpdater;

#define LOCKER QMutexLocker(&this->mutex)

class UpdateSchedulerPrivate : public UpdateScheduler{};
Q_GLOBAL_STATIC(UpdateSchedulerPrivate, privateInstance)

QString UpdateScheduler::tIndexToInfo(const std::type_index &info)
{
	return QString::fromLocal8Bit(info.name());
}

UpdateTask *UpdateScheduler::buildTask(const QString &info, const QByteArray &data)
{
	QMutexLocker(&privateInstance->mutex);

	Internal::UpdateTaskBuilder *builder = privateInstance->builderMap.value(info, NULL);
	if(builder)
		return builder->buildTask(data);
	else
		return NULL;
}

UpdateScheduler *UpdateScheduler::instance()
{
	return privateInstance;
}

bool UpdateScheduler::start(QSettings *settings)
{
	LOCKER;

	if(this->isActive)
		return false;
	this->isActive = true;

	if(!settings) {
		settings = new QSettings(this);
		settings->beginGroup(QStringLiteral("QtAutoUpdater/UpdateScheduler"));
	}
	this->settings = settings;
	this->settings->sync();

	int max = this->settings->beginReadArray(QStringLiteral("scheduleMemory"));
	for(int i = 0; i < max; ++i) {
		this->settings->setArrayIndex(i);

		QString info;
		info = this->settings->value(QStringLiteral("name")).toString();
		Internal::UpdateTaskBuilder *builder = this->builderMap.value(info, NULL);
		if(builder) {
			UpdateTask *task = builder->buildTask(this->settings->value(QStringLiteral("data")).toByteArray());
			int taskID = this->settings->value(QStringLiteral("taskID")).toInt();
			if(task && taskID)
				this->updateTasks.insert(task, taskID);
		}
	}
	this->settings->endArray();

	this->taskTimer = TimerObject::createTimer(this);
	QObject::connect(this->taskTimer, &TimerObject::taskFired,
					 this, &UpdateScheduler::taskFired,
					 Qt::QueuedConnection);
	QObject::connect(this->taskTimer, &TimerObject::taskDone,
					 this, &UpdateScheduler::taskDone,
					 Qt::QueuedConnection);

	for(UpdateTask *task : this->updateTasks.keys()) {
		QMetaObject::invokeMethod(this->taskTimer, "addTask", Qt::QueuedConnection,
								  Q_ARG(QtAutoUpdater::UpdateTask*, task));
	}

	return true;
}

bool UpdateScheduler::stop(bool writeSettings)
{
	LOCKER;

	if(!this->isActive)
		return false;

	this->taskTimer->destroyTimer();
	this->taskTimer = NULL;

	this->settings->remove(QStringLiteral("scheduleMemory"));
	if(writeSettings)
		this->settings->beginWriteArray(QStringLiteral("scheduleMemory"));
	int i = 0;
	typedef QHash<UpdateTask*, int>::iterator itr;
	for(itr it = this->updateTasks.begin(), end = this->updateTasks.end(); it != end; ++it) {
		if(writeSettings && it.key()->hasTasks()) {
			QByteArray data = it.key()->store();
			if(!data.isEmpty()) {
				this->settings->setArrayIndex(i++);

				this->settings->setValue(QStringLiteral("name"), UpdateScheduler::tIndexToInfo(it.key()->typeIndex()));
				this->settings->setValue(QStringLiteral("taskID"), it.value());

				this->settings->setValue(QStringLiteral("data"), data);
			}
		}
		delete it.key();
	}
	if(writeSettings)
		this->settings->endArray();
	this->settings->sync();
	this->settings->deleteLater();
	this->settings = NULL;

	this->updateTasks.clear();
	this->isActive = false;
	return true;
}

void UpdateScheduler::addBuilder(const QString &id, Internal::UpdateTaskBuilder *builder)
{
	LOCKER;

	delete this->builderMap.take(id);
	this->builderMap.insert(id, builder);
}

int UpdateScheduler::scheduleTask(UpdateTask *task)
{
	LOCKER;

	if(!this->isActive)
		this->start();

	int val;
	do {
		val = (qrand() << 16) + qrand() + 1;
	} while(this->updateTasks.values().contains(val));

	this->updateTasks.insert(task, val);
	QMetaObject::invokeMethod(this->taskTimer, "addTask", Qt::QueuedConnection,
							  Q_ARG(QtAutoUpdater::UpdateTask*, task));
	return val;
}

void UpdateScheduler::cancelTask(int taskID)
{
	LOCKER;

	UpdateTask *task = this->updateTasks.key(taskID, NULL);
	if(task) {
		QMetaObject::invokeMethod(this->taskTimer, "removeTask", Qt::QueuedConnection,
								  Q_ARG(QtAutoUpdater::UpdateTask*, task));
	}
}

void UpdateScheduler::taskFired(UpdateTask *task)
{
	LOCKER;

	int taskID = this->updateTasks.value(task, 0);
	if(taskID)
		emit taskReady(taskID);
}

void UpdateScheduler::taskDone(UpdateTask *task)
{
	LOCKER;

	int taskID = this->updateTasks.value(task, 0);
	if(taskID) {
		this->updateTasks.remove(task);
		delete task;
		emit taskFinished(taskID);
	}
}

UpdateScheduler::UpdateScheduler() :
	QObject(NULL),
	mutex(QMutex::NonRecursive),
	isActive(false),
	settings(NULL),
	builderMap(),
	updateTasks(),
	taskTimer(NULL)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());

	QObject::connect(qApp, &QCoreApplication::aboutToQuit,
					 this, [this](){
		this->stop(true);
	});
}

UpdateScheduler::~UpdateScheduler()
{
	qDeleteAll(this->builderMap.values());
}
