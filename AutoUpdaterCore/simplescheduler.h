#ifndef SIMPLESCHEDULER_H
#define SIMPLESCHEDULER_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QPair>
#include <QHash>

class SimpleScheduler : public QObject
{
	Q_OBJECT
public:
	explicit SimpleScheduler(QObject *parent = 0);

public slots:
	int startSchedule(int msecs, bool repeated = false, const QVariant &parameter = QVariant());
	int startSchedule(const QDateTime &when, const QVariant &parameter = QVariant());
	void cancelSchedule(int id);

signals:
	void scheduleTriggered(int id, const QVariant &parameter);

protected:
	void timerEvent(QTimerEvent *event) override;

private:
	typedef QPair<bool, QVariant> TimerInfo;

	QHash<int, TimerInfo> timerHash;
};

#endif // SIMPLESCHEDULER_H
