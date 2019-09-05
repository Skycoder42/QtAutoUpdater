#ifndef QTAUTOUPDATER_SIMPLESCHEDULER_P_H
#define QTAUTOUPDATER_SIMPLESCHEDULER_P_H

#include <chrono>

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QPair>
#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>

#include "qtautoupdatercore_global.h"

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT SimpleScheduler : public QObject
{
	Q_OBJECT

public:
	explicit SimpleScheduler(QObject *parent = nullptr);

public Q_SLOTS:
	int startSchedule(std::chrono::milliseconds msecs, bool repeated = false, const QVariant &parameter = {});
	int startSchedule(const QDateTime &when, const QVariant &parameter = {});
	void cancelSchedule(int id);

Q_SIGNALS:
	void scheduleTriggered(const QVariant &parameter);

protected:
	void timerEvent(QTimerEvent *event) override;

private:
	using TimerInfo = QPair<bool, QVariant>;

	QHash<int, TimerInfo> _timerHash;
};

Q_DECLARE_LOGGING_CATEGORY(logScheduler)

}

#endif // QTAUTOUPDATER_SIMPLESCHEDULER_P_H
