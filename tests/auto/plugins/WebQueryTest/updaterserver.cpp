#include "updaterserver.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <plugintest.h>

UpdaterServer::UpdaterServer(QObject *parent) :
	QObject{parent},
	_server{new QHttpServer{this}}
{}

bool UpdaterServer::create()
{
	TEST_WRAP_BEGIN

	QVERIFY(_server->route(QStringLiteral("/test-api/update-check"), [this](const QHttpServerRequest &request) -> QHttpServerResponse {
		auto ok = false;
		[&](){
			QCOMPARE(request.query(), _testQuery);
			ok = true;
		}();
		if (!ok)
			return QHttpServerResponse::StatusCode::BadRequest;

		if (_versionOnly) {
			if (_infos.first().version().isNull())
				return QHttpServerResponse::StatusCode::NoContent;
			else
				return _infos.first().version().toString();
		} else {
			QJsonArray infoArray;
			for (const auto &info : _infos) {
				QJsonObject infoObj;
				infoObj[QStringLiteral("name")] = info.name();
				infoObj[QStringLiteral("version")] = info.version().toString();
				infoObj[QStringLiteral("identifier")] = QJsonValue::fromVariant(info.identifier());
				infoObj[QStringLiteral("data")] = QJsonValue::fromVariant(info.data());
				infoArray.append(infoObj);
			}
			return infoArray;
		}
	}));

	QVERIFY(_server->route(QStringLiteral("/test-api/install"), [this](const QHttpServerRequest &request) -> QHttpServerResponse {
		QUrlQuery query{QString::fromUtf8(request.body())};
		if (query.queryItemValue(QStringLiteral("success")) == QVariant{true}) {
			const auto key = QStringLiteral("version");
			QCoreApplication::setApplicationVersion(query.queryItemValue(key));
			QUrlQuery newQuery;
			for (const auto &q : _testQuery.queryItems()) {
				if (q.first == key)
					newQuery.addQueryItem(key, query.queryItemValue(key));
				else
					newQuery.addQueryItem(q.first, q.second);
			}
			_testQuery = newQuery;
			_infos.clear();
			return QHttpServerResponse::StatusCode::NoContent;
		} else
			return QHttpServerResponse::StatusCode::NotAcceptable;
	}));

	QVERIFY(_server->route(QStringLiteral("/test-api/download/<arg>/<arg>"), [this](const QString &id, const QString &version) -> QHttpServerResponse {
		if (_infos.size() != 1)
			return QHttpServerResponse::StatusCode::BadRequest;
		if (_infos[0].identifier() != id)
			return QHttpServerResponse::StatusCode::BadRequest;
		if (_infos[0].version() != QVersionNumber::fromString(version))
			return QHttpServerResponse::StatusCode::BadRequest;
		return QHttpServerResponse::fromFile(QStringLiteral(SRCDIR "installer.py"));
	}));

	auto port = _server->listen(QHostAddress::LocalHost);
	QVERIFY(port != -1);
	_port = static_cast<quint16>(port);

	TEST_WRAP_END
}

QUrl UpdaterServer::checkUrl() const
{
	return QStringLiteral("http://localhost:%1/test-api/update-check").arg(_port);
}

QUrl UpdaterServer::installUrl() const
{
	return QStringLiteral("http://localhost:%1/test-api/install").arg(_port);
}

QString UpdaterServer::downloadUrl() const
{
	return QStringLiteral("http://localhost:%1/test-api/download/%{id}/%{version}").arg(_port);
}

void UpdaterServer::setUpdateInfo(QList<QtAutoUpdater::UpdateInfo> infos)
{
	_infos = std::move(infos);
	_versionOnly = false;
}

void UpdaterServer::setUpdateInfo(const QVersionNumber &version)
{
	_infos.clear();
	_infos.append({{}, {}, version});
	_versionOnly = true;
}

void UpdaterServer::setQuery(QUrlQuery query)
{
	_testQuery = std::move(query);
}
