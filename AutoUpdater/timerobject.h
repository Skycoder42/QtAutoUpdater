#ifndef TIMEROBJECT_H
#define TIMEROBJECT_H

#include <QObject>
#include <QHash>

namespace QtAutoUpdater
{
	class TimerObject : public QObject
	{
		Q_OBJECT
	public:
		static TimerObject *createTimer(QObject *threadParent);

	public slots:
		void addTask(int id, const QDateTime &timePoint);

	signals:
		void taskDone(int id);

	protected:
		void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;

	private:
		explicit TimerObject(QObject *parent = 0);

		QHash<int, int> idMap;
	};
}

#endif // TIMEROBJECT_H
