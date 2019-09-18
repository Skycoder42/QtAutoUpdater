#include "qwebqueryupdateinstaller.h"
#include <QtNetwork/QNetworkReply>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logWebInstaller, "qt.autoupdater.core.plugin.webquery.installer")

QWebQueryUpdateInstaller::QWebQueryUpdateInstaller(UpdaterBackend::IConfigReader *config, QNetworkAccessManager *nam, QObject *parent) :
	UpdateInstaller{parent},
	_config{config},
	_nam{nam}
{}

UpdateInstaller::Features QWebQueryUpdateInstaller::features() const
{
	return Feature::CanCancel |
			Feature::DetailedProgress |
			Feature::SelectComponents;
}

void QWebQueryUpdateInstaller::cancelInstall()
{
	abort(tr("Installation canceled by the user."));
}

void QWebQueryUpdateInstaller::eulaHandled(const QVariant &id, bool accepted)
{
}

void QWebQueryUpdateInstaller::restartApplication()
{

}

void QWebQueryUpdateInstaller::startInstallImpl()
{
	emit updateGlobalProgress(0.0, tr("Downloading update files…"));

	const auto allowInfoUrls = _config->value(QStringLiteral("installer/useInfoDownloads"), false).toBool();
	const auto baseUrl = _config->value(QStringLiteral("installer/downloadUrl"));
	for (const auto &info : components()) {
		auto useBase = true;
		if (allowInfoUrls) {
			const auto data = info.data();
			if (const auto key = QStringLiteral("download"); data.contains(key)) {
				useBase = false;
				const auto &downloads = data[key];
				QVariantList urlList;
				if (downloads.userType() == QMetaType::QVariantList)
					urlList = downloads.toList();
				else
					urlList.append(downloads);
				for (const auto &dlVar : urlList) {
					QUrl url;
					HashInfo hash;
					if (dlVar.userType() == QMetaType::QVariantMap) {
						const auto dlMap = dlVar.toMap();
						url = generateUrl(dlMap[QStringLiteral("url")].toString(), info);
						hash = extractHash(dlMap);
					} else
						url = dlVar.toUrl();

					if (url.isValid()) {
						if (url.isRelative())
							url = generateUrl(baseUrl->toString(), info).resolved(url);
						sendRequest(url, info, std::move(hash));
					} else
						qCWarning(logWebInstaller) << "Skipping invalid url from download info:" << dlVar;
				}
			}
		}

		if (useBase)
			sendRequest(generateUrl(baseUrl->toString(), info), info);
	}

	if (_repliesTotal == 0)
		emit installSucceeded(false);
}

void QWebQueryUpdateInstaller::replyDone()
{
	QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply {qobject_cast<QNetworkReply*>(sender())};
	if (!reply || !_replyCache.contains(reply.data()))
		return;
	auto [info, hashInfo, file] = _replyCache.take(reply.data());
	emit updateGlobalProgress(static_cast<double>(_replyCache.size()) / static_cast<double>(_repliesTotal), {});

	if (reply->error() != QNetworkReply::NoError) {
		qCCritical(logWebInstaller) << "Network request for"
									<< info.identifier()
									<< "failed with error:"
									<< qUtf8Printable(reply->errorString());
		file->deleteLater();
		abort(info.identifier(), tr("Network request failed with error: ") + reply->errorString());
		return;
	}

	const auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	switch (code) {
	case 200: // ok
	case 201: // created
		break;
	default:
		qCCritical(logWebInstaller) << "Network request for"
									<< info.identifier()
									<< "failed with HTTP status code:"
									<< code;
		file->deleteLater();
		abort(info.identifier(), tr("Network request failed with HTTP status code: %1").arg(code));
		return;
	}

	if (hashInfo) {
		if (hashInfo->first->result() != hashInfo->second) {
			qCCritical(logWebInstaller) << "Hashsums for" << info.identifier()
										<< "did not match! Expected" << hashInfo->second
										<< "but was" << hashInfo->first->result();
			file->deleteLater();
			abort(info.identifier(), tr("Hashsum verification failed! At least one download was corrupted."));
		}
		hashInfo = std::nullopt;
	}
	file->close();

	emit updateComponentProgress(info.identifier(), 1.0, tr("Download completed!"));

	if (_replyCache.isEmpty())
		finishInstall();
}

void QWebQueryUpdateInstaller::replyData()
{
	const auto reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply || !_replyCache.contains(reply))
		return;
	const auto &[info, hashInfo, file] = _replyCache[reply];

	while (reply->bytesAvailable() > 0) {
		const auto data = reply->read(1024);
		file->write(data);
		if (hashInfo)
			hashInfo->first->addData(data);
	}
}

void QWebQueryUpdateInstaller::replyProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	const auto reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply || !_replyCache.contains(reply))
		return;
	const auto &[info, hashInfo, file] = _replyCache[reply];

	if (bytesTotal == 0)
		emit updateComponentProgress(info.identifier(), -1.0, tr("Processing download…"));
	else if (bytesTotal < 0)
		emit updateComponentProgress(info.identifier(), -1.0, tr("Downloading update…"));
	else if (bytesReceived == bytesTotal)
		emit updateComponentProgress(info.identifier(), -1.0, tr("Processing downloaded data…"));
	else {
		emit updateComponentProgress(info.identifier(),
									 static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal),
									 tr("Downloading update…"));
	}
}

QUrl QWebQueryUpdateInstaller::generateUrl(QString base, const UpdateInfo &info)
{
	return base
			.replace(QStringLiteral("%{id}"), info.identifier().toString())
			.replace(QStringLiteral("%{version}"), info.version().toString())
			.replace(QStringLiteral("%{name}"), info.name());
}

QWebQueryUpdateInstaller::HashInfo QWebQueryUpdateInstaller::extractHash(const QVariantMap &data) const
{
	if (const auto key = QStringLiteral("sha3-512sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha3_512), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-384sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha3_384), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-256sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha3_256), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-224sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha3_224), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha512sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha512), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha384sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha384), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha256sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha256), data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha224sum"); data.contains(key))
		return std::make_pair(QSharedPointer<QCryptographicHash>::create(QCryptographicHash::Sha224), data.value(key).toByteArray());
	return std::nullopt;
}

void QWebQueryUpdateInstaller::sendRequest(const QUrl &url, UpdateInfo info, QWebQueryUpdateInstaller::HashInfo &&hashInfo)
{
	QNetworkRequest request{url};
	request.setAttribute(QNetworkRequest::SpdyAllowedAttribute, true);
	request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
	// optional ssl config
	if (const auto sslConf = _config->value(QStringLiteral("install/sslConfiguration")); sslConf)
		request.setSslConfiguration(sslConf->value<QSslConfiguration>());
	// optional headers
	if (const auto cnt = _config->value(QStringLiteral("install/headers/size")); cnt) {
		for (auto i = 0; i < cnt->toInt(); ++i) {
			request.setRawHeader(_config->value(QStringLiteral("install/headers/%1/key").arg(i))->toByteArray(),
								 _config->value(QStringLiteral("install/headers/%1/value").arg(i)).value_or(QVariant{}).toByteArray());
		}
	}

	auto tFile = new QTemporaryFile{this};
	if (!tFile->open()) {
		qCCritical(logWebInstaller) << "Unabled to create a temporary file for the download with error:"
									<< qUtf8Printable(tFile->errorString());
		tFile->deleteLater();
		abort(info.identifier(), tr("Unabled to cache downloaded updates!"));
		return;
	}
	auto reply = _nam->get(request);
	++_repliesTotal;
	_replyCache.insert(reply, std::make_tuple(std::move(info), std::move(hashInfo), tFile));
	connect(reply, &QNetworkReply::readyRead,
			this, &QWebQueryUpdateInstaller::replyData);
	connect(reply, &QNetworkReply::downloadProgress,
			this, &QWebQueryUpdateInstaller::replyProgress);
	connect(reply, &QNetworkReply::finished,
			this, &QWebQueryUpdateInstaller::replyDone,
			Qt::QueuedConnection);
	connect(this, &QWebQueryUpdateInstaller::cancelDownloads,
			reply, &QNetworkReply::abort);
}

void QWebQueryUpdateInstaller::finishInstall()
{
	emit updateGlobalProgress(-1.0, tr("Launching installer…"));
}

void QWebQueryUpdateInstaller::abort(const QVariant &id, const QString &message)
{
	emit updateComponentProgress(id, 1.0, message);
	abort(message);
}

void QWebQueryUpdateInstaller::abort(const QString &message)
{
	for (const auto &info : qAsConst(_replyCache))
		std::get<2>(info)->deleteLater();
	_replyCache.clear();
	emit cancelDownloads({});
	emit installFailed(message);
}
