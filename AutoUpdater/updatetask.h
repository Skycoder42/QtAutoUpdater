#ifndef UPDATETASK_H
#define UPDATETASK_H

#include <QDateTime>
#include <QVariant>

namespace QtAutoUpdater
{
	struct TimeSpan {
		quint64 count;
		enum TimeUnit : quint64 {
			MilliSeconds = 1ull,
			Seconds = MilliSeconds * 1000ull,
			Minutes = Seconds * 60ull,
			Hours = Minutes * 60ull,
			Days = Hours * 24ull,
			Weeks = Days * 7ull,
			Months = Days * 30ull,
			Years = Days * 365ull
		} unit;

		TimeSpan(quint64 count = 0, TimeUnit unit = MilliSeconds);

		quint64 msecs() const;
		QDateTime addToDateTime(const QDateTime &base) const;
	};

	class UpdateTask;
	class UpdateTaskBuilder
	{
	public:
		virtual inline ~UpdateTaskBuilder() {}
		virtual UpdateTask *buildTask(const QByteArray &data) = 0;
	};

	class UpdateTask
	{
	public:
		virtual inline ~UpdateTask() {}
		virtual bool hasTasks() const = 0;
		virtual QDateTime currentTask() const = 0;
		virtual bool nextTask() = 0;

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

		// UpdateTask interface
		bool hasTasks() const Q_DECL_OVERRIDE;
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

		// LoopUpdateTask interface
		qint64 repetitions() const Q_DECL_OVERRIDE;
		TimeSpan pauseSpan() const Q_DECL_OVERRIDE;
		QByteArray store() const Q_DECL_OVERRIDE;

	private:
		TimeSpan loopDelta;
		qint64 repCount;
	};

	class TimePointUpdateTask : public UpdateTask
	{
	public:
		TimePointUpdateTask(const QDateTime &timePoint, TimeSpan::TimeUnit repeatFocus = TimeSpan::MilliSeconds);
		TimePointUpdateTask(const QByteArray &data);

		bool hasTasks() const Q_DECL_OVERRIDE;
		QDateTime currentTask() const Q_DECL_OVERRIDE;
		bool nextTask() Q_DECL_OVERRIDE;
		QByteArray store() const Q_DECL_OVERRIDE;

	private:
		QDateTime timePoint;
		TimeSpan::TimeUnit focusPoint;

		QDateTime nextPoint;
	};
}

QDataStream &operator<<(QDataStream &stream, const QtAutoUpdater::TimeSpan &timeSpan);
QDataStream &operator>>(QDataStream &stream, QtAutoUpdater::TimeSpan &timeSpan);

#endif // UPDATETASK_H
