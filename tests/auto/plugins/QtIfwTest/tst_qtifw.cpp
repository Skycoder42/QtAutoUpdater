#include <plugintest.h>
#include "installercontroller.h"
using namespace QtAutoUpdater;

#define TEST_DELAY 100

class QtIfwTest : public PluginTest
{
	Q_OBJECT

protected:
	bool cleanup() override;
	QString backend() const override;
	QVariantMap config() override;
	QList<UpdateInfo> createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo) override;
	bool simulateInstall(const QVersionNumber &version) override;
	bool prepareUpdate(const QVersionNumber &version) override;
	bool canAbort(bool hard) const override;
	bool cancelState() const override;

private:
	QScopedPointer<InstallerController, QScopedPointerDeleteLater> controller;
};

bool QtIfwTest::cleanup()
{
	controller.reset();
	return true;
}

QString QtIfwTest::backend() const
{
	return QStringLiteral("qtifw");
}

QVariantMap QtIfwTest::config()
{
	return {
		{QStringLiteral("path"), controller->maintenanceToolPath() + QStringLiteral("/maintenancetool")},
		{QStringLiteral("silent"), true},
		{QStringLiteral("runAsAdmin"), false}
	};
}

QList<UpdateInfo> QtIfwTest::createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo)
{
	if (versionTo > versionFrom) {
		return {
			{
				QStringLiteral("QtAutoUpdaterTestInstaller"),
				QStringLiteral("QtAutoUpdaterTestInstaller"),
				versionTo,
				{
					{QStringLiteral("size"), 45ull}
				}
			}
		};
	} else
		return {};
}

bool QtIfwTest::simulateInstall(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN
	controller.reset(new InstallerController{this});
	QVERIFY(prepareUpdate(version));
	QVERIFY(controller->createInstaller());
	QVERIFY(controller->installLocal());
	TEST_WRAP_END
}

bool QtIfwTest::prepareUpdate(const QVersionNumber &version)
{
	TEST_WRAP_BEGIN
	controller->setVersion(version);
	QVERIFY(controller->createRepository());
	TEST_WRAP_END
}

bool QtIfwTest::canAbort(bool hard) const
{
#ifdef Q_OS_WIN
	return hard;
#else
	Q_UNUSED(hard)
	return true;
#endif
}

bool QtIfwTest::cancelState() const
{
	return false;
}

QTEST_GUILESS_MAIN(QtIfwTest)

#include "tst_qtifw.moc"
