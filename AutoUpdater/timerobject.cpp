#include "timerobject.h"
#include <QTimerEvent>
#include <QDateTime>
#include <QThread>
using namespace QtAutoUpdater;

TimerObject::TimerObject(QObject *parent) :
	QObject(parent),
	idMap()
{}

TimerObject *TimerObject::createTimer(QObject *threadParent)
{
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

void TimerObject::addTask(int id, const QDateTime &timePoint)
{
	int tId = this->startTimer(QDateTime::currentDateTime().msecsTo(timePoint),
							   Qt::PreciseTimer);
	if(tId != 0)
		this->idMap.insert(tId, id);
	else
		emit taskDone(id);
}


void QtAutoUpdater::TimerObject::timerEvent(QTimerEvent *event)
{
	this->killTimer(event->timerId());
	emit taskDone(this->idMap.take(event->timerId()));
	event->accept();
}
