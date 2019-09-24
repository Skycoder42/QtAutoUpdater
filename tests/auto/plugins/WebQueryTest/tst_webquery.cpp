class WebQueryTest;
#define TEST_FRIEND WebQueryTest;

#include <plugintest.h>
#include <QtGui/QGuiApplication>
#include "updaterserver.h"
using namespace QtAutoUpdater;

class WebQueryTest : public PluginTest
{
	Q_OBJECT

protected:
	bool init() override;
	bool cleanup() override;
	QString backend() const override;
	QVariantMap config() override;
	QVariantMap performConfig() override;
	QList<UpdateInfo> createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo) override;
	bool simulateInstall(const QVersionNumber &version) override;
	bool prepareUpdate(const QVersionNumber &version) override;
	bool canAbort(bool hard) const override;
	bool cancelState() const override;

private Q_SLOTS:
	void testSpecialUpdates_data();
	void testSpecialUpdates();

private:
	UpdaterServer *_server = nullptr;
	QString _parser = QStringLiteral("auto");
	bool _autoQuery = true;
};

bool WebQueryTest::init()
{
	TEST_WRAP_BEGIN

	_server = new UpdaterServer{this};
	QVERIFY(_server->create());

	TEST_WRAP_END
}

bool WebQueryTest::cleanup()
{
	_server->deleteLater();
	_server = nullptr;
	return true;
}

QString WebQueryTest::backend() const
{
	return QStringLiteral("webquery");
}

QVariantMap WebQueryTest::config()
{
	return {
		{QStringLiteral("check/url"), _server->checkUrl()},
		{QStringLiteral("check/autoQuery"), _autoQuery},
		{QStringLiteral("check/parser"), _parser},

#ifdef Q_OS_WIN
		{QStringLiteral("install/tool"), QStringLiteral("python")},
#else
		{QStringLiteral("install/tool"), QStringLiteral("python3")},
#endif
		{QStringLiteral("install/parallel"), true},
		{QStringLiteral("install/arguments"), QStringList {
			QStringLiteral(SRCDIR "installer.py"),
			_server->installUrl().toString(QUrl::FullyEncoded)
		}},
		{QStringLiteral("install/addDataArgs"), true},
		{QStringLiteral("install/runAsAdmin"), false}
	};
}

QVariantMap WebQueryTest::performConfig()
{
	return {
		{QStringLiteral("check/url"), _server->checkUrl()},
		{QStringLiteral("check/autoQuery"), _autoQuery},
		{QStringLiteral("check/parser"), _parser},

		{QStringLiteral("install/download"), true},
		{QStringLiteral("install/downloadUrl"), _server->downloadUrl()},
#ifdef Q_OS_WIN
		{QStringLiteral("install/tool"), QStringLiteral("python")},
#else
		{QStringLiteral("install/tool"), QStringLiteral("python3")},
#endif
		{QStringLiteral("install/parallel"), true},
		{QStringLiteral("install/arguments"), QStringList {
			QStringLiteral("%{downloadPath}"),
			_server->installUrl().toString(QUrl::FullyEncoded)
		}},
		{QStringLiteral("install/addDataArgs"), true},
		{QStringLiteral("install/runAsAdmin"), false}
	};
}

QList<UpdateInfo> WebQueryTest::createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo)
{
	if (versionTo > versionFrom) {
		if (_parser == QStringLiteral("version")) {
			return {
				{
					QCoreApplication::applicationName(),
					QGuiApplication::applicationDisplayName(),
					versionTo
				}
			};
		} else {
			return {
				{
					QStringLiteral("test-update"),
					QStringLiteral("test-update"),
					versionTo,
					{
						{QStringLiteral("arguments"), QStringList {
							QVariant{true}.toString(),
							versionTo.toString()
						}},
						{QStringLiteral("eulas"), QVariantList {
							QStringLiteral("EULA 1"),
							QVariantMap {
								{QStringLiteral("text"), QStringLiteral("EULA 2")},
								{QStringLiteral("required"), true},
							},
							QVariantMap {
								{QStringLiteral("text"), QStringLiteral("EULA 3")},
								{QStringLiteral("required"), false},
							}
						}}
					}
				}
			};
		}
	} else
		return {};
}

bool WebQueryTest::simulateInstall(const QVersionNumber &version)
{
	QCoreApplication::setApplicationVersion(version.toString());
	return true;
}

bool WebQueryTest::prepareUpdate(const QVersionNumber &version)
{
	if (_parser == QStringLiteral("version")) {
		_server->setUpdateInfo(version > QVersionNumber::fromString(QCoreApplication::applicationVersion()) ?
								   version :
								   QVersionNumber{});
	} else {
		_server->setUpdateInfo(createInfos(QVersionNumber::fromString(QCoreApplication::applicationVersion()),
										   version));
	}
	QUrlQuery query;
	if (_autoQuery) {
		query.addQueryItem(QStringLiteral("name"), QCoreApplication::applicationName());
		query.addQueryItem(QStringLiteral("version"), QCoreApplication::applicationVersion());
		query.addQueryItem(QStringLiteral("domain"), QCoreApplication::organizationDomain());
		query.addQueryItem(QStringLiteral("abi"), QSysInfo::buildAbi());
		query.addQueryItem(QStringLiteral("kernel-type"), QSysInfo::kernelType());
		query.addQueryItem(QStringLiteral("kernel-version"), QSysInfo::kernelVersion());
		query.addQueryItem(QStringLiteral("os-type"), QSysInfo::productType());
		query.addQueryItem(QStringLiteral("os-version"), QSysInfo::productVersion());
	}
	_server->setQuery(query);
	return true;
}

bool WebQueryTest::canAbort(bool hard) const
{
	Q_UNUSED(hard)
	return true;
}

bool WebQueryTest::cancelState() const
{
	return false;
}

void WebQueryTest::testSpecialUpdates_data()
{
	QTest::addColumn<QVersionNumber>("installedVersion");
	QTest::addColumn<QVersionNumber>("updateVersion");
	QTest::addColumn<int>("abortLevel");
	QTest::addColumn<bool>("success");

	QTest::addColumn<bool>("simpleVersion");
	QTest::addColumn<bool>("noQuery");

	QTest::newRow("complexVersion.noUpdates") << QVersionNumber(1, 0, 0)
											  << QVersionNumber(1, 0, 0)
											  << 0
											  << true
											  << false
											  << false;

	QTest::newRow("complexVersion.simpleUpdate") << QVersionNumber(1, 0, 0)
												 << QVersionNumber(1, 1, 0)
												 << 0
												 << true
												 << false
												 << false;

	QTest::newRow("simpleVersion.noUpdates") << QVersionNumber(1, 0, 0)
											 << QVersionNumber(1, 0, 0)
											 << 0
											 << true
											 << true
											 << false;

	QTest::newRow("simpleVersion.simpleUpdate") << QVersionNumber(1, 0, 0)
												<< QVersionNumber(1, 1, 0)
												<< 0
												<< true
												<< true
												<< false;

	QTest::newRow("noQuery.noUpdates") << QVersionNumber(1, 0, 0)
									   << QVersionNumber(1, 0, 0)
									   << 0
									   << true
									   << false
									   << true;

	QTest::newRow("noQuery.simpleUpdate") << QVersionNumber(1, 0, 0)
										  << QVersionNumber(1, 1, 0)
										  << 0
										  << true
										  << false
										  << true;
}

void WebQueryTest::testSpecialUpdates()
{
	QFETCH(bool, simpleVersion);
	QFETCH(bool, noQuery);

	_parser = simpleVersion ? QStringLiteral("version") : QStringLiteral("json");
	_autoQuery = !noQuery;

	testUpdateCheck();
}

QTEST_GUILESS_MAIN(WebQueryTest)

#include "tst_webquery.moc"
