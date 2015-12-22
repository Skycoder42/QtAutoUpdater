#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QDateTime>
#if QT_VERSION >= 0x050600
#include <QVersionNumber>
#else
struct QVersionNumber : public QString {
	QVersionNumber(const QString &str = QString()) : QString(str) {}

	inline QString toString() const {return (*this);}
	static inline QVersionNumber fromString(const QString &string, int * = 0) {return QVersionNumber(string);}
};
#endif
#include <QDebug>

class AutoUpdaterPrivate;
class AutoUpdater : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath WRITE setMaintenanceToolPath)
	Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
	Q_PROPERTY(QList<UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)

public:
	struct UpdateInfo
	{
		QString name;
		QVersionNumber version;
		quint64 size;

		UpdateInfo();
		UpdateInfo(const UpdateInfo &other);
		UpdateInfo(QString name, QVersionNumber version, quint64 size);
	};

	explicit AutoUpdater(QObject *parent = NULL);
	~AutoUpdater();

	bool exitedNormally() const;
	int getErrorCode() const;
	QByteArray getErrorLog() const;

	QString maintenanceToolPath() const;
	void setMaintenanceToolPath(QString maintenanceToolPath);
	bool isRunning() const;
	QList<UpdateInfo> updateInfo() const;

public slots:
	bool checkForUpdates();
	void abortUpdateCheck(int maxDelay = 0);

	//IDEA advance managing using an extra class
	int scheduleUpdate(qint64 mDelay, bool repeated = false, Qt::TimerType timerType = Qt::PreciseTimer);
	inline int Q_CONSTEXPR scheduleUpdate(const QDateTime &when) {
		return this->scheduleUpdate(QDateTime::currentDateTime().msecsTo(when), false);
	}
	bool cancelScheduledUpdate(int taskId);

	inline void Q_CONSTEXPR runUpdaterOnExit(bool runAsAdmin = false) {
		this->runUpdaterOnExit({QStringLiteral("--updater")}, runAsAdmin);
	}
	void runUpdaterOnExit(const QStringList &arguments, bool runAsAdmin = false);

signals:
	void checkUpdatesDone(bool hasUpdates, bool hasError);

	void runningChanged(bool running);
	void updateInfoChanged(QList<AutoUpdater::UpdateInfo> updateInfo);

private:
	AutoUpdaterPrivate *d_ptr;
	Q_DECLARE_PRIVATE(AutoUpdater)
};

Q_DECLARE_METATYPE(AutoUpdater::UpdateInfo)

QDebug &operator<<(QDebug &debug, const AutoUpdater::UpdateInfo &info);

#endif // AUTOUPDATER_H
