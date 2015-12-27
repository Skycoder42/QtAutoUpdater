#include "timerobject.h"
#include <QTimerEvent>
#include <QDateTime>
#include <QThread>
#include <QDebug>
using namespace QtAutoUpdater;

TimerObject::TimerObject(QObject *parent) :
	QObject(parent),
	taskMap()
{}

TimerObject *TimerObject::createTimer(QObject *threadParent)
{
	Q_ASSERT(threadParent);
	QThread *timerThread = new QThread(threadParent);

	TimerObject *timer = new TimerObject(NULL);
	timer->moveToThread(timerThread);
	connect(timerThread, &QThread::finished,
			timer, &TimerObject::deleteLater,
			Qt::DirectConnection);

	connect(threadParent, &QObject::destroyed, timerThread, [=](){
		timerThread->quit();
		if(!timerThread->wait(500))
			timerThread->terminate();
	}, Qt::DirectConnection);

	timerThread->start(QThread::HighPriority);
	return timer;
}

void TimerObject::destroyTimer()
{
	this->disconnect(SIGNAL(taskFired(QtAutoUpdater::UpdateTask*)));
	this->disconnect(SIGNAL(taskDone(QtAutoUpdater::UpdateTask*)));
	QThread *thread = this->thread();
	thread->quit();
	if(!thread->wait(5000))
		thread->terminate();
	thread->deleteLater();
}

void TimerObject::addTask(UpdateTask *task)
{
	Q_ASSERT(task);
	if(!task->hasTasks())
		emit taskDone(task);
	else {
		int tId = this->startTimer(QDateTime::currentDateTime().msecsTo(task->currentTask()),
								   Qt::PreciseTimer);
		if(tId != 0)
			this->taskMap.insert(tId, task);
		else
			emit taskDone(task);
	}
}

void TimerObject::removeTask(UpdateTask *task)
{
	Q_ASSERT(task);
	int id = this->taskMap.key(task, 0);
	if(id != 0) {
		this->taskMap.remove(id);
		emit taskDone(task);
	}
}

void TimerObject::timerEvent(QTimerEvent *event)
{
	this->killTimer(event->timerId());
	UpdateTask *task = this->taskMap.take(event->timerId());
	event->accept();

	if(task) {
		emit taskFired(task);
		if(task->nextTask())
			this->addTask(task);
		else
			emit taskDone(task);
	}
}
