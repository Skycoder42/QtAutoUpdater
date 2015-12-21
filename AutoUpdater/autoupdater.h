#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#if QT_VERSION >= 0x050600
#include <QVersionNumber>
#else
struct QVersionNumber : public QString {
	QVersionNumber(const QString &str) : QString(str) {}

	inline QString toString() const {return (*this);}
	static inline QVersionNumber fromString(const QString &string, int * = 0) {return QVersionNumber(string);}
};
#endif

class AutoUpdaterPrivate;
class AutoUpdater : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath WRITE setMaintenanceToolPath)
	Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
	Q_PROPERTY(QStringList updateArguments READ updateArguments WRITE setUpdateArguments)
	Q_PROPERTY(bool runAsAdmin READ runAsAdmin WRITE setRunAsAdmin)
	Q_PROPERTY(QList<UpdateInfo> updateInfo READ updateInfo NOTIFY updateInfoChanged)

public:
	struct UpdateInfo
	{
		QString name;
		QVersionNumber version;
		quint64 size;
	};

	explicit AutoUpdater(QObject *parent = NULL);
	~AutoUpdater();

	int getErrorCode() const;
	QString getErrorLog() const;

	QString maintenanceToolPath() const;
	bool isRunning() const;
	QStringList updateArguments() const;
	bool runAsAdmin() const;
	QList<UpdateInfo> updateInfo() const;

public slots:
	bool checkForUpdates(int delay = 0);

	void setMaintenanceToolPath(QString maintenanceToolPath);
	void setUpdateArguments(QStringList updateArguments);
	void setRunAsAdmin(bool runAsAdmin);

signals:
	void checkUpdatesDone(bool hasUpdates);

	void runningChanged(bool running);
	void updateInfoChanged(QList<UpdateInfo> updateInfo);

private:
	AutoUpdaterPrivate *d_ptr;
	Q_DECLARE_PRIVATE(AutoUpdater)
};



#endif // AUTOUPDATER_H
