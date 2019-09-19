#ifndef UPDATERSERVER_H
#define UPDATERSERVER_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtHttpServer/QHttpServer>

class UpdaterServer : public QObject
{
	Q_OBJECT

public:
	explicit UpdaterServer(QObject *parent = nullptr);

	bool create();

	QUrl checkUrl() const;
	QUrl installUrl() const;
	QString downloadUrl() const;

	void setUpdateInfo(QList<QtAutoUpdater::UpdateInfo> infos);
	void setUpdateInfo(const QVersionNumber &version);
	void setQuery(QUrlQuery query);

private:
	QHttpServer *_server;
	quint16 _port;

	QList<QtAutoUpdater::UpdateInfo> _infos;
	bool _versionOnly = false;
	QUrlQuery _testQuery;
};

#endif // UPDATERSERVER_H
