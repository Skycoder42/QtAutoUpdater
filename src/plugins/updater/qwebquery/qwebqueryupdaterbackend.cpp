#include "qwebqueryupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QSysInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHttpMultiPart>
#include <QtGui/QGuiApplication>
#include <QtGui/QDesktopServices>

#if QT_CONFIG(process)
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtAutoUpdaterCore/ProcessBackend>
#include <QtAutoUpdaterCore/AdminAuthoriser>
#include "qwebqueryupdateinstaller.h"
#endif
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logWebBackend, "qt.autoupdater.core.plugin.webquery.backend")

const QString QWebQueryUpdaterBackend::ParserAuto = QStringLiteral("auto");
const QString QWebQueryUpdaterBackend::ParserVersion = QStringLiteral("version");
const QString QWebQueryUpdaterBackend::ParserJson = QStringLiteral("json");

QWebQueryUpdaterBackend::QWebQueryUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

UpdaterBackend::Features QWebQueryUpdaterBackend::features() const
{
	Features features = Feature::CheckProgress;
	if (config()->value(QStringLiteral("install/download"), false).toBool())
		features |= Feature::PerformInstall;
	else if (config()->value(QStringLiteral("install/parallel"), false).toBool())
		features |= Feature::ParallelTrigger;
	else if (config()->value(QStringLiteral("install/tool"), false).toBool())
		features |= Feature::TriggerInstall;
	return features;
}

void QWebQueryUpdaterBackend::checkForUpdates()
{
	const auto urlVariant = config()->value(QStringLiteral("check/url"));
	if (!urlVariant) {
		emit checkDone(false);
		return;
	}

	auto url = urlVariant->toUrl();
	if (config()->value(QStringLiteral("check/autoQuery"), true).toBool())
		addStandardQuery(url);
	QNetworkRequest request{url};
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	request.setAttribute(QNetworkRequest::SpdyAllowedAttribute, true);
	request.setAttribute(QNetworkRequest::HTTP2AllowedAttribute, true);
	// optional ssl config
	if (const auto sslConf = config()->value(QStringLiteral("check/sslConfiguration")); sslConf)
		request.setSslConfiguration(sslConf->value<QSslConfiguration>());
	// optional headers
	if (const auto cnt = config()->value(QStringLiteral("check/headers/size")); cnt) {
		for (auto i = 0; i < cnt->toInt(); ++i) {
			request.setRawHeader(config()->value(QStringLiteral("check/headers/%1/key").arg(i))->toByteArray(),
								 config()->value(QStringLiteral("check/headers/%1/value").arg(i)).value_or(QVariant{}).toByteArray());
		}
	}

	// figure out the verb and body and send the request
	QNetworkReply *reply = nullptr;
	const auto verb = config()->value(QStringLiteral("check/verb"), QByteArray{"GET"}).toByteArray();
	if (const auto body = config()->value(QStringLiteral("check/body")); body) {
		auto bodyObj = body->value<QObject*>();
		if (bodyObj) {
			if (bodyObj->metaObject()->inherits(&QIODevice::staticMetaObject))
				reply = _nam->sendCustomRequest(request, verb, static_cast<QIODevice*>(bodyObj));
			else if (bodyObj->metaObject()->inherits(&QHttpMultiPart::staticMetaObject))
				reply = _nam->sendCustomRequest(request, verb, static_cast<QHttpMultiPart*>(bodyObj));
			else
				emit checkDone(false);
		} else
			reply = _nam->sendCustomRequest(request, verb, body->toByteArray());
	} else
		reply = _nam->sendCustomRequest(request, verb);
	if (!reply)
		return;

	connect(reply, &QNetworkReply::downloadProgress,
			this, &QWebQueryUpdaterBackend::downloadProgress);
	connect(reply, &QNetworkReply::finished,
			this, &QWebQueryUpdaterBackend::handleReply,
			Qt::QueuedConnection);  // make queued to ensure executed after downloadProgress
	connect(this, &QWebQueryUpdaterBackend::abortCheck,
			reply, &QNetworkReply::abort);
}

void QWebQueryUpdaterBackend::abort(bool force)
{
	Q_UNUSED(force)
	emit abortCheck({});
}

bool QWebQueryUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
#if QT_CONFIG(process)
	if (const auto program = testForProcess(config()); program) {
		auto sigMethodIdx = metaObject()->indexOfSignal("triggerInstallDone(bool)");
		Q_ASSERT(sigMethodIdx != -1);
		return runProcess(this, metaObject()->method(sigMethodIdx), config(), *program, infos, track);
	}
#endif

	if (auto url = config()->value(QStringLiteral("install/url"), {}).toUrl(); url.isValid()) {
		if (config()->value(QStringLiteral("install/addDataArgs"), false).toBool()) {
			QUrlQuery query{url};
			for (const auto &info : infos) {
				auto data = info.data();
				if (const auto key = QStringLiteral("query"); data.contains(key)) {
					const auto infoQuery = data.value(key).toMap();
					for (auto it = infoQuery.begin(), end = infoQuery.end(); it != end; ++it) {
						if (it.key().startsWith(QLatin1Char('+'))) {
							const auto rKey = it.key().mid(1);
							query.addQueryItem(rKey, query.queryItemValue(rKey, QUrl::FullyDecoded) + it->toString());
						} else
							query.addQueryItem(it.key(), it->toString());
					}
				}
			}
			url.setQuery(query);
		}
		return QDesktopServices::openUrl(url);
	} else {
		qCCritical(logWebBackend) << "Unable to find an updater to execute!";
		return false;
	}
}

UpdateInstaller *QWebQueryUpdaterBackend::createInstaller()
{
#if QT_CONFIG(process)
	return new QWebQueryUpdateInstaller{config(), _nam, this};
#else
	return nullptr;
#endif
}

bool QWebQueryUpdaterBackend::initialize()
{
	_nam = new QNetworkAccessManager{this};
	_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
	_nam->setStrictTransportSecurityEnabled(config()->value(QStringLiteral("check/hsts"), true).toBool());  // TODO test
	return true;
}

void QWebQueryUpdaterBackend::handleReply()
{
	QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply {qobject_cast<QNetworkReply*>(sender())};
	if (!reply)
		return;

	if (reply->error() != QNetworkReply::NoError) {
		qCCritical(logWebBackend) << "Network request for"
								  << requestUrl(reply.data())
								  << "failed with error:"
								  << qUtf8Printable(reply->errorString());
		emit checkDone(false);
		return;
	}

	const auto code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	switch (code) {
	case 200: // ok
	case 201: // created
		parseResult(reply.data());
		break;
	case 204:  // no content
		emit checkDone(true);
		break;
	default:
		qCCritical(logWebBackend) << "Network request for"
								  << requestUrl(reply.data())
								  << "failed with HTTP status code:"
								  << code;
		emit checkDone(false);
		break;
	}
}

void QWebQueryUpdaterBackend::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (bytesTotal == 0)
		emit checkProgress(-1.0, tr("Processing reply…"));
	else if (bytesTotal < 0)
		emit checkProgress(-1.0, tr("Downloading update infos…"));
	else if (bytesReceived == bytesTotal)
		emit checkProgress(-1.0, tr("Processing downloaded data…"));
	else {
		emit checkProgress(static_cast<double>(bytesReceived) / static_cast<double>(bytesTotal),
						   tr("Downloading update infos…"));
	}
}

void QWebQueryUpdaterBackend::addStandardQuery(QUrl &url) const
{
	QUrlQuery query{url};
	if (const auto key = QStringLiteral("name"); !query.hasQueryItem(key))
		query.addQueryItem(key, QCoreApplication::applicationName());
	if (const auto key = QStringLiteral("version"); !query.hasQueryItem(key))
		query.addQueryItem(key, QCoreApplication::applicationVersion());
	if (const auto key = QStringLiteral("domain"); !query.hasQueryItem(key))
		query.addQueryItem(key, QCoreApplication::organizationDomain());
	if (const auto key = QStringLiteral("abi"); !query.hasQueryItem(key))
		query.addQueryItem(key, QSysInfo::buildAbi());
	if (const auto key = QStringLiteral("os-type"); !query.hasQueryItem(key))
		query.addQueryItem(key, QSysInfo::productType());
	if (const auto key = QStringLiteral("os-version"); !query.hasQueryItem(key))
		query.addQueryItem(key, QSysInfo::productVersion());
	url.setQuery(query);
}

void QWebQueryUpdaterBackend::parseResult(QNetworkReply *reply)
{
	const auto parser = config()->value(QStringLiteral("check/parser"), ParserAuto).toString();
	if (parser == ParserAuto) {
		const auto contentType = reply->header(QNetworkRequest::ContentTypeHeader).toByteArray();
		if (contentType == "application/json")
			parseJson(reply);
		else if (contentType == "text/plain")
			parseVersion(reply);
		else {
			qCCritical(logWebBackend) << "Unable to determined parser type from content type:" << contentType.constData();
			emit checkDone(false);
		}
	} else if (parser == ParserJson)
		parseJson(reply);
	else if (parser == ParserVersion)
		parseVersion(reply);
	else {
		qCCritical(logWebBackend) << "Unknown web reply parser type:" << qUtf8Printable(parser);
		emit checkDone(false);
	}
}

void QWebQueryUpdaterBackend::parseJson(QNetworkReply *reply)
{
	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(reply->readAll(), &error);
	if (error.error != QJsonParseError::NoError) {
		qCCritical(logWebBackend) << "Network request for"
								  << requestUrl(reply)
								  << "returned invalid JSON data. Parser error:"
								  << qUtf8Printable(error.errorString());
		emit checkDone(false);
		return;
	}

	QJsonArray data;
	if (doc.isArray())
		data = doc.array();
	else
		data.append(doc.object());

	QList<UpdateInfo> infos;
	infos.reserve(data.size());
	for (const auto &value : qAsConst(data)) {
		const auto obj = value.toObject();
		UpdateInfo info;
		info.setName(obj[QStringLiteral("name")].toString());
		info.setVersion(QVersionNumber::fromString(obj[QStringLiteral("version")].toString()));
		info.setSize(static_cast<quint64>(obj[QStringLiteral("size")].toDouble()));
		info.setIdentifier(obj[QStringLiteral("identifier")].toVariant());
		info.setData(obj[QStringLiteral("data")].toVariant().toMap());
		infos.append(info);
	}
	emit checkDone(true, infos);
}

void QWebQueryUpdaterBackend::parseVersion(QNetworkReply *reply)
{
	QTextStream stream{reply};
	auto version = QVersionNumber::fromString(stream.readAll());
	if (version.isNull()) {
		qCCritical(logWebBackend) << "Network request for"
								  << requestUrl(reply)
								  << "returned an invalid version number!";
		emit checkDone(false);
	} else if (version > QVersionNumber::fromString(QCoreApplication::applicationVersion())) {
		UpdateInfo info;
		info.setName(QGuiApplication::applicationDisplayName());
		info.setVersion(std::move(version));
		info.setIdentifier(QCoreApplication::applicationName());
		emit checkDone(true, {info});
	} else
		emit checkDone(true);
}

QString QWebQueryUpdaterBackend::requestUrl(QNetworkReply *reply) const
{
	return static_cast<QNetworkReply*>(reply)->request().url().adjusted(QUrl::RemovePassword).toString();
}

#if QT_CONFIG(process)
std::optional<QString> QWebQueryUpdaterBackend::testForProcess(IConfigReader *config)
{
	auto tool = config->value(QStringLiteral("install/tool"));
	if (!tool)
		return std::nullopt;
	QStringList paths;
	if (const auto mPaths = config->value(QStringLiteral("install/path")); mPaths)
		paths = ProcessBackend::readPathList(*mPaths);
	const auto fullExe = QStandardPaths::findExecutable(tool->toString(), paths);
	if (QFileInfo{fullExe}.isExecutable())
		return fullExe;
	else
		return std::nullopt;
}

bool QWebQueryUpdaterBackend::runProcess(QObject *parent, QMetaMethod doneSignal, IConfigReader *config, const QString &program, const QList<UpdateInfo> &infos, bool track)
{
	QStringList args;
	if (const auto mArgs = config->value(QStringLiteral("install/arguments")); mArgs)
		args = ProcessBackend::readArgumentList(*mArgs);
	if (config->value(QStringLiteral("install/addDataArgs"), false).toBool()) {
		for (const auto &info : infos) {
			auto data = info.data();
			if (const auto key = QStringLiteral("arguments"); data.contains(key))
				args += ProcessBackend::readArgumentList(data.value(key));
		}
	}

	QString pwd;
	if (const auto mPwd = config->value(QStringLiteral("install/pwd")); mPwd) {
		pwd = mPwd->toString();
		if (pwd.isEmpty())
			pwd = QCoreApplication::applicationDirPath();
	}

	if (config->value(QStringLiteral("install/runAsAdmin"), AdminAuthoriser::needsAdminPermission(program)).toBool()) {
		if (track)
			qCWarning(logWebBackend) << "Unable to track progress of application executed as root/admin! It will be run detached instead";
		const auto ok =  AdminAuthoriser::executeAsAdmin(program, args, pwd);
		if (ok && track) { // invoke queued to make shure is emitted AFTER the start install signal in the updater
			QMetaObject::invokeMethod(parent, "triggerInstallDone", Qt::QueuedConnection,
									  Q_ARG(bool, true));
		}
		return ok;
	} else {
		auto proc = new QProcess{parent};
		proc->setProgram(program);
		proc->setArguments(args);
		if (!pwd.isEmpty())
			proc->setWorkingDirectory(pwd);
		if (track) {
			proc->setProcessChannelMode(QProcess::ForwardedErrorChannel);
			proc->setStandardOutputFile(QProcess::nullDevice());
			proc->setStandardInputFile(QProcess::nullDevice());
			connect(proc, &QProcess::stateChanged, parent, [parent, doneSignal, proc, program](QProcess::ProcessState state) {
				if (state == QProcess::NotRunning) {
					switch (proc->exitStatus()) {
					case QProcess::NormalExit:
						if (const auto code = proc->exitCode(); code != EXIT_SUCCESS)
							qCWarning(logWebBackend) << program << "exited with unclean exit code" << code;
						doneSignal.invoke(parent, Q_ARG(bool, true));
						break;
					case QProcess::CrashExit:
						qCCritical(logWebBackend) << "Failed to run"
												  << program <<
													 "- crashed with error:"
												  << qUtf8Printable(proc->errorString());
						doneSignal.invoke(parent, Q_ARG(bool, false));
						break;
					}
				}
				proc->deleteLater();
			});
			proc->start(QIODevice::ReadOnly);
			return true;
		} else {
			const auto ok = proc->startDetached();
			proc->deleteLater();
			if (!ok)
				qCCritical(logWebBackend) << "Failed to start" << program << "to install updates";
			return ok;
		}
	}
}
#endif
