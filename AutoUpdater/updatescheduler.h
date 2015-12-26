#ifndef UPDATESCHEDULER_H
#define UPDATESCHEDULER_H

#include <QObject>
#include <QDateTime>
#include <QVector>

namespace QtAutoUpdater
{
	struct RepetiveUpdateTask
	{
		QDateTime startPoint;
		qint64 repetitions;
		struct TimeSpan {
			quint64 num;
			enum TimeType : quint64 {
				MilliSeconds = 1ull,
				Seconds = MilliSeconds * 1000ull,
				Minutes = Seconds * 60ull,
				Hours = Minutes * 60ull,
				Days = Hours * 24ull,
				Weeks = Days * 7ull,
				Months = Days * 30ull,
				Years = Days * 365ull
			} type;
		} pauseDelay;
	};

	class UpdateSchedulerPrivate;
	class UpdateScheduler : public QObject
	{
		Q_OBJECT
	public:
		static UpdateScheduler *instance();

	public slots:

	signals:

	private:
		UpdateScheduler(UpdateSchedulerPrivate *d_ptr);

		UpdateSchedulerPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateScheduler)
	};
}

#endif // UPDATESCHEDULER_H
