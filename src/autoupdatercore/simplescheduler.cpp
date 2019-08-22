#include "simplescheduler_p.h"
#include "updater_p.h"

#include <QtCore/QTimerEvent>
#include <QtCore/QDebug>

using namespace QtAutoUpdater;

SimpleScheduler::SimpleScheduler(QObject *parent) :
	QObject{parent}
{}

int SimpleScheduler::startSchedule(int msecs, bool repeated, const QVariant &parameter)
{
	if(msecs < 0) {
		qCWarning(logQtAutoUpdater) << "Cannot schedule update tasks for the past!";
		return 0;
	}

	const auto id = startTimer(msecs, Qt::VeryCoarseTimer);
	if(id != 0)
		_timerHash.insert(id, {repeated, parameter});
	return id;
}

int SimpleScheduler::startSchedule(const QDateTime &when, const QVariant &parameter)
{
	const auto delta = QDateTime::currentDateTime().msecsTo(when);
	if(delta > static_cast<qint64>(std::numeric_limits<int>::max())) {
		qCWarning(logQtAutoUpdater) << "Time interval to big, timepoint to far in the future.";
		return 0;
	} else
		return startSchedule(static_cast<int>(delta), false, parameter);
}

void SimpleScheduler::cancelSchedule(int id)
{
	killTimer(id);
	_timerHash.remove(id);
}

void SimpleScheduler::timerEvent(QTimerEvent *event)
{
	const auto id = event->timerId();
	const auto info = _timerHash.value(id, {false, QVariant()});
	if(!info.first)
		cancelSchedule(id);
	emit scheduleTriggered(info.second);
	event->accept();
}
