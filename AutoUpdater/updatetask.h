#ifndef UPDATETASK_H
#define UPDATETASK_H

#include <QDateTime>
#include <QVariant>
#include <QLinkedList>
#include <typeindex>

namespace QtAutoUpdater
{
	//! A basic class that provides a timespan
	struct TimeSpan {
		//! The "number" of that timespan
		quint64 count;
		//! An enum that lists the different possible units for timespans
		enum TimeUnit : quint64 {
			MilliSeconds = 1ull, //!< `count` measures milliseconds
			Seconds = MilliSeconds * 1000ull, //!< `count` measures seconds (1000 milliseconds)
			Minutes = Seconds * 60ull, //!< `count` measures minutes (60 seconds)
			Hours = Minutes * 60ull, //!< `count` measures hours (60 minutes)
			Days = Hours * 24ull, //!< `count` measures days (24 hours)
			Weeks = Days * 7ull, //!< `count` measures weeks (7 days)
			Months = Days * 30ull, //!< `count` measures months (30 days)
			Years = Days * 365ull //!< `count` measures years (365 days)
		} unit;//!< The unit of the timespan

		//! Constructs a timespan based on a count and unit
		TimeSpan(quint64 count = 0, TimeUnit unit = MilliSeconds);

		//! returns this timespan in milliseconds
		quint64 msecs() const;
		//! adds this timespan to a datetime-object
		QDateTime addToDateTime(const QDateTime &base = QDateTime::currentDateTime()) const;
	};

	//! An interface for tasks to be handeled by the UpdateScheduler
	class UpdateTask
	{
	public:
		//! Virtual destructor
		virtual inline ~UpdateTask() {}
		//! Returns whether there are tasks left or not
		virtual bool hasTasks() = 0;
		//! The current task to be scheduled for this object
		virtual QDateTime currentTask() const = 0;
		//! Tries to move on to the next task, if present
		virtual bool nextTask() = 0;

		//! Returns the type_index of the class
		virtual std::type_index typeIndex() const = 0;
		//! Stores the UpdateTask into a bytearray to be saved
		virtual QByteArray store() const = 0;
	};

	class LoopUpdateTask : public UpdateTask
	{
	public:
		virtual inline TimeSpan startDelay() const {
			return this->pauseSpan();
		}
		virtual qint64 repetitions() const = 0;
		virtual TimeSpan pauseSpan() const = 0;

		bool hasTasks() Q_DECL_OVERRIDE;
		QDateTime currentTask() const Q_DECL_OVERRIDE;
		bool nextTask() Q_DECL_OVERRIDE;

	private:
		QDateTime nextPoint;
		qint64 repetitionsLeft;
	};

	class BasicLoopUpdateTask : public LoopUpdateTask
	{
	public:
		BasicLoopUpdateTask(TimeSpan loopDelta, qint64 repeats = -1);
		BasicLoopUpdateTask(const QByteArray &data);

		qint64 repetitions() const Q_DECL_OVERRIDE;
		TimeSpan pauseSpan() const Q_DECL_OVERRIDE;
		QByteArray store() const Q_DECL_OVERRIDE;

		std::type_index typeIndex() const Q_DECL_OVERRIDE;

	private:
		TimeSpan loopDelta;
		qint64 repCount;
	};

	class TimePointUpdateTask : public UpdateTask
	{
	public:
		TimePointUpdateTask(const QDateTime &timePoint, TimeSpan::TimeUnit repeatFocus = TimeSpan::MilliSeconds);
		TimePointUpdateTask(const QByteArray &data);

		bool hasTasks() Q_DECL_OVERRIDE;
		QDateTime currentTask() const Q_DECL_OVERRIDE;
		bool nextTask() Q_DECL_OVERRIDE;
		QByteArray store() const Q_DECL_OVERRIDE;

		std::type_index typeIndex() const Q_DECL_OVERRIDE;

	private:
		QDateTime timePoint;
		TimeSpan::TimeUnit focusPoint;

		QDateTime nextPoint;
	};

	class UpdateTaskList : public QLinkedList<UpdateTask*>, public UpdateTask
	{
	public:
		UpdateTaskList();
		UpdateTaskList(std::initializer_list<UpdateTask*> list);
		UpdateTaskList(const QByteArray &data);

		bool hasTasks() Q_DECL_OVERRIDE;
		QDateTime currentTask() const Q_DECL_OVERRIDE;
		bool nextTask() Q_DECL_OVERRIDE;
		QByteArray store() const Q_DECL_OVERRIDE;

		std::type_index typeIndex() const Q_DECL_OVERRIDE;
	};
}

Q_DECLARE_METATYPE(QtAutoUpdater::UpdateTask*)

QDataStream &operator<<(QDataStream &stream, const QtAutoUpdater::TimeSpan &timeSpan);
QDataStream &operator>>(QDataStream &stream, QtAutoUpdater::TimeSpan &timeSpan);

#endif // UPDATETASK_H
