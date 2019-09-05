#include "simplescheduler_p.h"
#include "updater_p.h"

#include <QtCore/QTimerEvent>
#include <QtCore/QDebug>
using namespace QtAutoUpdater;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logScheduler, "qt.autoupdater.core.SimpleScheduler")

}

SimpleScheduler::SimpleScheduler(QObject *parent) :
	QObject{parent}
{}

int SimpleScheduler::startSchedule(std::chrono::milliseconds msecs, bool repeated, const QVariant &parameter)
{
	if(msecs < 0ms) {
		qCWarning(logScheduler) << "Cannot schedule update tasks for the past!";
		return 0;
	}

	const auto id = startTimer(msecs, Qt::VeryCoarseTimer);
	if(id != 0)
		_timerHash.insert(id, {repeated, parameter});
	return id;
}

int SimpleScheduler::startSchedule(const QDateTime &when, const QVariant &parameter)
{
	const milliseconds delta {QDateTime::currentDateTime().msecsTo(when)};
	return startSchedule(delta, false, parameter);
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
