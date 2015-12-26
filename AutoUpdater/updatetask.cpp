#include "updatetask.h"
#include <QDataStream>
using namespace QtAutoUpdater;

#define NOW QDateTime::currentDateTime()

//-------- TimeSpan --------

TimeSpan::TimeSpan(quint64 count, TimeSpan::TimeUnit unit) :
	count(count),
	unit(unit)
{}

quint64 TimeSpan::msecs() const {
	return this->count * this->unit;
}

QDateTime TimeSpan::addToDateTime(const QDateTime &base) const
{
	switch(this->unit) {
	case MilliSeconds:
		return base.addMSecs((qint64)this->count);
	case Seconds:
		return base.addSecs((qint64)this->count);
	case Minutes:
		return base.addSecs((qint64)this->count * 60ll);
	case Hours:
		return base.addSecs((qint64)this->count * 3600ll);
	case Days:
		return base.addDays((qint64)this->count);
	case Weeks:
		return base.addDays((qint64)this->count * 7ll);
	case Months:
		return base.addMonths((int)this->count);
	case Years:
		return base.addYears((int)this->count);
	default:
		Q_UNREACHABLE();
		return base;
	}
}

QDataStream &operator<<(QDataStream &stream, const QtAutoUpdater::TimeSpan &timeSpan)
{
	stream << timeSpan.count
		   << timeSpan.unit;
	return stream;
}

QDataStream &operator>>(QDataStream &stream, QtAutoUpdater::TimeSpan &timeSpan)
{
	quint64 tmp;
	stream >> timeSpan.count
		   >> tmp;
	timeSpan.unit = (TimeSpan::TimeUnit)tmp;
	return stream;
}

//-------- LoopUpdateTask --------

bool LoopUpdateTask::hasTasks() const
{
	return (this->repetitionsLeft != 0 &&
			this->nextPoint > NOW);
}

QDateTime LoopUpdateTask::currentTask() const
{
	return this->nextPoint;
}

bool LoopUpdateTask::nextTask()
{
	if(this->repetitionsLeft != 0) {
		if(this->repetitionsLeft > 0)
			--this->repetitionsLeft;
		this->nextPoint = this->pauseSpan().addToDateTime(NOW);
		return true;
	} else
		return false;
}

//-------- DelayedLoopUpdateTask --------

BasicLoopUpdateTask::BasicLoopUpdateTask(TimeSpan loopDelta, qint64 repeats) :
	LoopUpdateTask(),
	loopDelta(loopDelta),
	repCount(repeats)
{}

BasicLoopUpdateTask::BasicLoopUpdateTask(const QByteArray &data) :
	LoopUpdateTask(),
	loopDelta(),
	repCount(-1)
{
	QDataStream stream(data);
	stream >> this->loopDelta
		   >> this->repCount;
}

qint64 BasicLoopUpdateTask::repetitions() const
{
	return this->repCount;
}

TimeSpan BasicLoopUpdateTask::pauseSpan() const
{
	return this->loopDelta;
}

QByteArray BasicLoopUpdateTask::store() const
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream << this->loopDelta
		   << this->repCount;
	return data;
}

//-------- TimePointUpdateTask --------

TimePointUpdateTask::TimePointUpdateTask(const QDateTime &timePoint, TimeSpan::TimeUnit repeatFocus) :
	timePoint(timePoint),
	focusPoint(repeatFocus),
	nextPoint(timePoint)
{}

TimePointUpdateTask::TimePointUpdateTask(const QByteArray &data) :
	timePoint(),
	focusPoint(TimeSpan::MilliSeconds),
	nextPoint()
{
	QDataStream stream(data);
	quint64 tmp;
	stream >> this->timePoint
		   >> tmp;
	this->focusPoint = (TimeSpan::TimeUnit)tmp;
	this->nextPoint = this->timePoint;
}

bool TimePointUpdateTask::hasTasks() const
{
	if(this->focusPoint == TimeSpan::Years)
		return this->timePoint > NOW;
	else
		return true;
}

QDateTime TimePointUpdateTask::currentTask() const
{
	return this->nextPoint;
}

bool TimePointUpdateTask::nextTask()
{
	switch(this->focusPoint) {
	case TimeSpan::MilliSeconds:
		return false;
	case TimeSpan::Seconds:
		this->nextPoint = this->nextPoint.addSecs(1);
		break;
	case TimeSpan::Minutes:
		this->nextPoint = this->nextPoint.addSecs(60);
		break;
	case TimeSpan::Hours:
		this->nextPoint = this->nextPoint.addSecs(3600);
		break;
	case TimeSpan::Days:
		this->nextPoint = this->nextPoint.addDays(1);
		break;
	case TimeSpan::Weeks:
		this->nextPoint = this->nextPoint.addDays(7);
		break;
	case TimeSpan::Months:
		this->nextPoint = this->nextPoint.addMonths(1);
		break;
	case TimeSpan::Years:
		this->nextPoint = this->nextPoint.addYears(1);
		break;
	default:
		Q_UNREACHABLE();
		return false;
	}

	return true;
}

QByteArray TimePointUpdateTask::store() const
{
	QByteArray data;
	QDataStream stream(&data, QIODevice::WriteOnly);
	stream << this->timePoint
		   << this->focusPoint;
	return data;
}
