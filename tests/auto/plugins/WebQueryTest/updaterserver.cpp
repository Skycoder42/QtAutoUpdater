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
			QCOMPARE(QUrlQuery{request.url()}, _testQuery);
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
				infoObj[QStringLiteral("size")] = static_cast<double>(info.size());
				infoObj[QStringLiteral("identifier")] = QJsonValue::fromVariant(info.identifier());
				infoObj[QStringLiteral("data")] = QJsonValue::fromVariant(info.data());
				infoArray.append(infoObj);
			}
			return infoArray;
		}
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

void UpdaterServer::setUpdateInfo(QList<QtAutoUpdater::UpdateInfo> infos)
{
	_infos = std::move(infos);
	_versionOnly = false;
}

void UpdaterServer::setUpdateInfo(const QVersionNumber &version)
{
	_infos.clear();
	_infos.append({QString{}, version});
	_versionOnly = true;
}

void UpdaterServer::setQuery(QUrlQuery query)
{
	_testQuery = query;
}
