#include <plugintest.h>
#include "testinstaller.h"
using namespace QtAutoUpdater;

class HomebrewTest : public PluginTest
{
	Q_OBJECT

protected:
	bool init() override;
	bool cleanup() override;
	QString backend() const override;
	QVariantMap config() override;
	QList<UpdateInfo> createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo) override;
	bool simulateInstall(const QVersionNumber &version) override;
	bool prepareUpdate(const QVersionNumber &version) override;
	bool canAbort(bool hard) const override;
	bool cancelState() const override;

private:
	TestInstaller *_installer = nullptr;
};

bool HomebrewTest::init()
{
	TEST_WRAP_BEGIN

	_installer = new TestInstaller{this};
	QVERIFY(_installer->setup());

	TEST_WRAP_END
}

bool HomebrewTest::cleanup()
{
	TEST_WRAP_BEGIN

	QVERIFY(_installer->cleanup());
	_installer->deleteLater();
	_installer = nullptr;

	TEST_WRAP_END
}

QString HomebrewTest::backend() const
{
	return QStringLiteral("homebrew");
}

QVariantMap HomebrewTest::config()
{
	return {
		{QStringLiteral("packages"), QStringLiteral("skycoder42/qtautoupdatertest/qtautoupdatertestpackage")}
	};
}

QList<UpdateInfo> HomebrewTest::createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo)
{
	if (versionTo > versionFrom) {
		return {
			{
				QStringLiteral("skycoder42/qtautoupdatertest/qtautoupdatertestpackage"),
				QStringLiteral("skycoder42/qtautoupdatertest/qtautoupdatertestpackage"),
				versionTo,
				{
					{QStringLiteral("oldVersions"), QVariantList{QVariant::fromValue(versionFrom)}}
				}
			}
		};
	} else
		return {};
}

bool HomebrewTest::simulateInstall(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN

	if (_installer->isInstalled())
		QVERIFY(_installer->uninstall());
	QVERIFY(prepareUpdate(version));
	QVERIFY(_installer->install());

	TEST_WRAP_END
}

bool HomebrewTest::prepareUpdate(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN

	_installer->setVersion(version);
	QVERIFY(_installer->package());

	TEST_WRAP_END
}

bool HomebrewTest::canAbort(bool hard) const
{
	return hard;
}

bool HomebrewTest::cancelState() const
{
	return false;
}

QTEST_GUILESS_MAIN(HomebrewTest)

#include "tst_homebrew.moc"
