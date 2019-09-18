#ifndef QWEBQUERYUPDATEINSTALLER_H
#define QWEBQUERYUPDATEINSTALLER_H

#include <QtCore/QHash>
#include <QtCore/QCryptographicHash>
#include <QtCore/QTemporaryFile>

#include <QtAutoUpdaterCore/UpdateInstaller>
#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtNetwork/QNetworkAccessManager>

class QWebQueryUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	explicit QWebQueryUpdateInstaller(QtAutoUpdater::UpdaterBackend::IConfigReader *config,
									  QNetworkAccessManager *nam,
									  QObject *parent = nullptr);

	Features features() const override;

public Q_SLOTS:
	void cancelInstall() override;
	void eulaHandled(const QVariant &id, bool accepted) override;
	void restartApplication() override;

Q_SIGNALS:
	void cancelDownloads(QPrivateSignal);

protected:
	void startInstallImpl() override;

private Q_SLOTS:
	void replyDone();
	void replyData();
	void replyProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
	using HashInfo = std::optional<std::pair<QSharedPointer<QCryptographicHash>, QByteArray>>;
	using DownloadInfo = std::tuple<QtAutoUpdater::UpdateInfo, HashInfo, QTemporaryFile*>;

	QtAutoUpdater::UpdaterBackend::IConfigReader *_config;
	QNetworkAccessManager *_nam;

	quint64 _repliesTotal = 0;
	QHash<QNetworkReply*, DownloadInfo> _replyCache;

	QUrl generateUrl(QString base, const QtAutoUpdater::UpdateInfo &info);
	HashInfo extractHash(const QVariantMap &data) const;
	void sendRequest(const QUrl &url, QtAutoUpdater::UpdateInfo info, HashInfo &&hashInfo = std::nullopt);
	void finishInstall();

	void abort(const QVariant &id, const QString &message);
	void abort(const QString &message);
};

Q_DECLARE_LOGGING_CATEGORY(logWebInstaller)

#endif // QWEBQUERYUPDATEINSTALLER_H
