#include <plugintest.h>
#include "testinstaller.h"
using namespace QtAutoUpdater;

class ChocolateyTest : public PluginTest
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
	TestInstaller *_installer;
};

bool ChocolateyTest::init()
{
	_installer = new TestInstaller{this};
	return _installer->setup();
}

bool ChocolateyTest::cleanup()
{
	_installer->deleteLater();
	if (_installer->isInstalled())
		return _installer->uninstall();
	else
		return true;
}

QString ChocolateyTest::backend() const
{
	return QStringLiteral("chocolatey");
}

QVariantMap ChocolateyTest::config()
{
	return {
		{QStringLiteral("packages"), QStringLiteral("qtautoupdater-test-package")},
		{QStringLiteral("extraCheckArgs"), QStringList {
			QStringLiteral("-s"),
			_installer->repositoryPath()
		}}
	};
}

QList<UpdateInfo> ChocolateyTest::createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo)
{
	if (versionTo > versionFrom) {
		return {
			{
				QStringLiteral("qtautoupdater-test-package"),
				QStringLiteral("qtautoupdater-test-package"),
				versionTo,
				{
					{"oldVersion", QVariant::fromValue(versionFrom)}
				}
			}
		};
	} else
		return {};
}

bool ChocolateyTest::simulateInstall(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN

	if (_installer->isInstalled())
		QVERIFY(_installer->uninstall());
	QVERIFY(prepareUpdate(version));
	QVERIFY(_installer->install());

	TEST_WRAP_END
}

bool ChocolateyTest::prepareUpdate(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN

	_installer->setVersion(version);
	QVERIFY(_installer->package());

	TEST_WRAP_END
}

bool ChocolateyTest::canAbort(bool hard) const
{
	return hard;
}

bool ChocolateyTest::cancelState() const
{
	return false;
}

QTEST_GUILESS_MAIN(ChocolateyTest)

#include "tst_chocolatey.moc"

