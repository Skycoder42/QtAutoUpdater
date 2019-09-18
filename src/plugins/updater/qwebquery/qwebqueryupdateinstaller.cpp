#include "qwebqueryupdateinstaller.h"
#include "qwebqueryupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QProcess>
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
	return Feature::CanCancel;
}

void QWebQueryUpdateInstaller::cancelInstall()
{
	abort();
}

void QWebQueryUpdateInstaller::eulaHandled(const QVariant &id, bool accepted)
{
}

void QWebQueryUpdateInstaller::restartApplication()
{
	if (QProcess::startDetached(QCoreApplication::applicationFilePath(),
								QCoreApplication::arguments(),
								QDir::currentPath()))
		QCoreApplication::quit();
	else
		qCCritical(logWebInstaller) << "Failed to restart the application";
}

void QWebQueryUpdateInstaller::startInstallImpl()
{
	// load component
	auto comps = components();
	if (comps.size() != 1) {
		qCCritical(logWebInstaller) << "Can only install a singular update!";
		abort(tr("Invalid update details!"));
		return;
	}
	_info = comps[0];
	const auto data = _info.data();

	// TODO handle eulas

	// get the download url
	auto url = generateUrl(_config->value(QStringLiteral("installer/downloadUrl")));
	if (_config->value(QStringLiteral("installer/useInfoDownloads"), false).toBool()) {
		if (const auto key = QStringLiteral("download"); data.contains(key)) {
			auto infoUrl = data[key].toUrl();
			if (infoUrl.isRelative() && url)
				url = url->resolved(infoUrl);
		}
	}
	if (!url) {
		qCCritical(logWebInstaller) << "Unable to generate the download URL!";
		abort(tr("Invalid update details!"));
		return;
	}

	// extract hashsum info
	if (auto hash = extractHash(data); hash) {
		_hash.reset(new QCryptographicHash{hash->first});
		_hashResult = std::move(hash->second);
	}

	// prepare the download file
	const auto fileNamePattern = QDir::temp().absoluteFilePath(QStringLiteral("qtautoupdater_XXXXXX.%1"))
								 .arg(QFileInfo{url->path(QUrl::FullyDecoded)}.completeSuffix());
	_file = new QTemporaryFile{fileNamePattern, this};
	if (!_file->open()) {
		qCCritical(logWebInstaller) << "Unabled to create a temporary file for the download with error:"
									<< qUtf8Printable(_file->errorString());
		abort(tr("Unabled to cache downloaded updates!"));
		return;
	}

	emit updateGlobalProgress(0.0, tr("Downloading update files…"));

	QNetworkRequest request{*url};
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

	auto reply = _nam->get(request);
	connect(this, &QWebQueryUpdateInstaller::cancelDownloads,
			reply, &QNetworkReply::abort);
	connect(reply, &QNetworkReply::readyRead,
			this, &QWebQueryUpdateInstaller::replyData);
	connect(reply, &QNetworkReply::downloadProgress,
			this, &QWebQueryUpdateInstaller::replyProgress);
	connect(reply, &QNetworkReply::finished,
			this, &QWebQueryUpdateInstaller::replyDone,
			Qt::QueuedConnection);
}

void QWebQueryUpdateInstaller::replyDone()
{
	QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply {qobject_cast<QNetworkReply*>(sender())};
	if (!reply)
		return;

	reply->disconnect(this);
	this->disconnect(reply.data());
	if (reply->error() != QNetworkReply::NoError) {
		qCCritical(logWebInstaller) << "Network request for"
									<< _info.identifier()
									<< "failed with error:"
									<< qUtf8Printable(reply->errorString());
		abort(tr("Network request failed with error: %1").arg(reply->errorString()));
		return;
	}

	const auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	switch (code) {
	case 200: // ok
	case 201: // created
		break;
	default:
		qCCritical(logWebInstaller) << "Network request for"
									<< _info.identifier()
									<< "failed with HTTP status code:"
									<< code;
		abort(tr("Network request failed with HTTP status code: %1").arg(code));
		return;
	}

	if (_hash) {
		if (_hash->result() != _hashResult) {
			qCCritical(logWebInstaller) << "Hashsums for" << _info.identifier()
										<< "did not match! Expected" << _hashResult.toHex()
										<< "but was" << _hash->result().toHex();
			abort(tr("Hashsum verification failed! At least one download was corrupted."));
		}
		_hash.reset();
		_hashResult.clear();
	}
	_file->close();

	finishInstall();
}

void QWebQueryUpdateInstaller::replyData()
{
	const auto reply = qobject_cast<QNetworkReply*>(sender());
	if (!reply)
		return;

	while (reply->bytesAvailable() > 0) {
		const auto data = reply->read(1024);
		if (_file)
			_file->write(data);
		if (_hash)
			_hash->addData(data);
	}
}

void QWebQueryUpdateInstaller::replyProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (bytesTotal == 0)
		emit updateGlobalProgress(-1.0, tr("Processing download…"));
	else if (bytesTotal < 0)
		emit updateGlobalProgress( -1.0, tr("Downloading update…"));
	else if (bytesReceived == bytesTotal)
		emit updateGlobalProgress(-1.0, tr("Processing downloaded data…"));
	else {
		emit updateGlobalProgress(static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal),
								  tr("Downloading update…"));
	}
}

void QWebQueryUpdateInstaller::installerDone(bool success)
{
	if (success)
		emit installSucceeded(true);
	else
		emit installFailed(tr("Update installer tool failed!"));
}

std::optional<QUrl> QWebQueryUpdateInstaller::generateUrl(const std::optional<QVariant> &base)
{
	if (base) {
		return base->toString()
				.replace(QStringLiteral("%{id}"), _info.identifier().toString())
				.replace(QStringLiteral("%{version}"), _info.version().toString())
				.replace(QStringLiteral("%{name}"), _info.name());
	} else
		return std::nullopt;
}

QWebQueryUpdateInstaller::HashInfo QWebQueryUpdateInstaller::extractHash(const QVariantMap &data) const
{
	if (const auto key = QStringLiteral("sha3-512sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha3_512, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-384sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha3_384, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-256sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha3_256, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha3-224sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha3_224, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha512sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha512, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha384sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha384, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha256sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha256, data.value(key).toByteArray());
	if (const auto key = QStringLiteral("sha224sum"); data.contains(key))
		return std::make_pair(QCryptographicHash::Sha224, data.value(key).toByteArray());
	return std::nullopt;
}

void QWebQueryUpdateInstaller::finishInstall()
{
	emit updateGlobalProgress(-1.0, tr("Launching installer…"));

	// find the executable
	std::optional<QString> program;
	if (_config->value(QStringLiteral("installer/execDownload"), false).toBool() &&
		_info.data().value(QStringLiteral("exec"), false).toBool()) {
		program = _file->fileName();
		if (!_file->setPermissions(_file->permissions() | QFileDevice::ExeUser)) {
			qCCritical(logWebBackend) << "Failed to make" << *program << "executable";
			abort(tr("Failed to make downloaded file executable!"));
			return;
		}
	} else
		program = QWebQueryUpdaterBackend::testForProcess(_config);
	if (!program) {
		qCCritical(logWebBackend) << "Downloaded data is not executable and no tool was specified";
		abort(tr("Unable to find executable to install the update!"));
		return;
	}

	// connect if parallel is allowed
	QMetaMethod trackSignal;
	if (_config->value(QStringLiteral("install/parallel"), false).toBool()) {
		auto sigMethodIdx = metaObject()->indexOfSignal("installerDone(bool)");
		Q_ASSERT(sigMethodIdx != -1);
		trackSignal = metaObject()->method(sigMethodIdx);
	}

	// execute the installer
	const auto ok = QWebQueryUpdaterBackend::runProcess(this, trackSignal, _config, *program, {_info}, trackSignal.isValid());
	if (ok) {
		_file->setAutoRemove(false);
		_file->deleteLater();
		_file = nullptr;
		if (!trackSignal.isValid())
			QCoreApplication::quit();  // exit because parallel install is not allowed
	} else
		abort(tr("Failed to start updater program!"));
}

void QWebQueryUpdateInstaller::abort(const QString &message)
{
	if (_file) {
		_file->setAutoRemove(true);
		_file->close();
		_file->deleteLater();
		_file = nullptr;
	}
	_hash.reset();
	_hashResult.clear();
	emit cancelDownloads({});
	if (!message.isEmpty())
		emit installFailed(message);
}
