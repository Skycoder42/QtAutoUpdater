#ifndef SIMPLESCHEDULER_P_H
#define SIMPLESCHEDULER_P_H

#include "qautoupdatercore_global.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QPair>
#include <QtCore/QHash>

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT SimpleScheduler : public QObject
{
	Q_OBJECT

public:
	explicit SimpleScheduler(QObject *parent = nullptr);

public Q_SLOTS:
	int startSchedule(int msecs, bool repeated = false, const QVariant &parameter = QVariant());
	int startSchedule(const QDateTime &when, const QVariant &parameter = QVariant());
	void cancelSchedule(int id);

Q_SIGNALS:
	void scheduleTriggered(const QVariant &parameter);

protected:
	void timerEvent(QTimerEvent *event) override;

private:
	typedef QPair<bool, QVariant> TimerInfo;

	QHash<int, TimerInfo> timerHash;
};

}

#endif // SIMPLESCHEDULER_P_H
