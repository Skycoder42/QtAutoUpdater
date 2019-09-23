#ifndef QWEBQUERYUPDATEINSTALLER_H
#define QWEBQUERYUPDATEINSTALLER_H

#include <QtCore/QHash>
#include <QtCore/QCryptographicHash>
#include <QtCore/QTemporaryFile>
#include <QtCore/QSet>

#include <QtAutoUpdaterCore/UpdateInstaller>
#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtNetwork/QNetworkAccessManager>

#include "qwebqueryupdaterbackend.h"

class QWebQueryUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	using Check = QWebQueryUpdaterBackend::Check;
	using Install = QWebQueryUpdaterBackend::Install;

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
	void installerDone(bool success);

	void startDownload();

private:
	using HashInfo = std::optional<std::pair<QCryptographicHash::Algorithm, QByteArray>>;

	QtAutoUpdater::UpdaterBackend::IConfigReader *_config;
	QNetworkAccessManager *_nam;

	QtAutoUpdater::UpdateInfo _info;
	QSet<QUuid> _eulaCache;
	QScopedPointer<QCryptographicHash> _hash;
	QByteArray _hashResult;
	QTemporaryFile *_file = nullptr;

	std::optional<QUrl> generateUrl(const std::optional<QVariant> &base);
	HashInfo extractHash(const QVariantMap &data) const;
	void finishInstall();

	void abort(const QString &message = {});
};

Q_DECLARE_LOGGING_CATEGORY(logWebInstaller)

#endif // QWEBQUERYUPDATEINSTALLER_H
