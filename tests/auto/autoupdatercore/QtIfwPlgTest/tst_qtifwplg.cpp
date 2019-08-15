#include <QtTest>
#include <QtAutoUpdaterCore>
#include "installercontroller.h"
using namespace QtAutoUpdater;

#define TEST_DELAY 1000


class QtIfwPlgTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testUpdateCheck_data();
	void testUpdateCheck();

	void testUpdateInstall();

private:
	InstallerController *controller = nullptr;
};

void QtIfwPlgTest::initTestCase()
{
	controller = new InstallerController(this);
	controller->createRepository();
	controller->createInstaller();
	controller->installLocal();
}

void QtIfwPlgTest::cleanupTestCase()
{
	delete controller;
	controller = nullptr;
}

void QtIfwPlgTest::testUpdateCheck_data()
{
	QTest::addColumn<QVersionNumber>("repoVersion");
	QTest::addColumn<bool>("hasUpdates");
	QTest::addColumn<QList<UpdateInfo>>("updates");

	QList<UpdateInfo> updates;

	QTest::newRow("noUpdates") << QVersionNumber(1, 0, 0)
							   << false
							   << updates;

	UpdateInfo info;
	info.setName(QStringLiteral("QtAutoUpdaterTestInstaller"));
	info.setVersion(QVersionNumber::fromString(QStringLiteral("1.1.0")));
	info.setSize(45ull);
	updates.append(info);
	QTest::newRow("simpleUpdate") << QVersionNumber(1, 1, 0)
								  << true
								  << updates;

	updates.clear();
}

void QtIfwPlgTest::testUpdateCheck()
{
	QFETCH(QVersionNumber, repoVersion);
	QFETCH(bool, hasUpdates);
	QFETCH(QList<UpdateInfo>, updates);

	controller->setVersion(repoVersion);
	controller->createRepository();

	auto updater = Updater::createUpdater(QStringLiteral("qtifw"), {
											  {QStringLiteral("path"), controller->maintenanceToolPath() + QStringLiteral("/maintenancetool")}
										  }, this);
	QVERIFY(updater);
	QSignalSpy checkSpy(updater, &Updater::checkUpdatesDone);
	QVERIFY(checkSpy.isValid());
	QSignalSpy runningSpy(updater, &Updater::runningChanged);
	QVERIFY(runningSpy.isValid());
	QSignalSpy updateInfoSpy(updater, &Updater::updateInfoChanged);
	QVERIFY(updateInfoSpy.isValid());

	//start the check updates
	QVERIFY(!updater->isRunning());
	updater->checkForUpdates();

	//runnig should have changed to true
	QCOMPARE(runningSpy.size(), 1);
	QVERIFY(runningSpy.takeFirst()[0].toBool());
	QVERIFY(updater->isRunning());
	QVERIFY(updateInfoSpy.takeFirst()[0].value<QList<UpdateInfo>>().isEmpty());

	//wait max 5 min for the process to finish
	QVERIFY(checkSpy.wait(300000));

	//check if the finished signal is without error
	QCOMPARE(checkSpy.size(), 1);
	QCOMPARE(checkSpy.takeFirst()[0].value<Updater::Result>(), hasUpdates ? Updater::Result::NewUpdates : Updater::Result::NoUpdates);
	QCOMPARE(updater->updateInfo(), updates);
	if(hasUpdates) {
		QCOMPARE(updateInfoSpy.size(), 1);
		QCOMPARE(updateInfoSpy.takeFirst()[0].value<QList<UpdateInfo>>(), updates);
	}

	//runnig should have changed to false
	QCOMPARE(runningSpy.size(), 1);
	QVERIFY(!runningSpy.takeFirst()[0].toBool());
	QVERIFY(!updater->isRunning());

	//verifiy all signalspies are empty
	QVERIFY(checkSpy.isEmpty());
	QVERIFY(runningSpy.isEmpty());
	QVERIFY(updateInfoSpy.isEmpty());

	//-----------schedule mechanism---------------

	int kId = updater->scheduleUpdate(1, true);//every 1 minute
	QVERIFY(kId);
	updater->cancelScheduledUpdate(kId);

	kId = updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(5));
	QVERIFY(updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(2)));
	updater->cancelScheduledUpdate(kId);

	//wait for the update to start
	QVERIFY(runningSpy.wait(2000 + TEST_DELAY));
	//should be running
	QVERIFY(runningSpy.size() > 0);
	QVERIFY(runningSpy.takeFirst()[0].toBool());
	//wait for it to finish if not running
	if(runningSpy.isEmpty())
		QVERIFY(runningSpy.wait(120000));
	//should have stopped
	QCOMPARE(runningSpy.size(), 1);
	QVERIFY(!runningSpy.takeFirst()[0].toBool());

	//wait for the canceled one (max 5 secs)
	QVERIFY(!runningSpy.wait(5000 + TEST_DELAY));

	//verifiy the runningSpy is empty
	QVERIFY(runningSpy.isEmpty());
	//clear the rest
	checkSpy.clear();
	updateInfoSpy.clear();

	delete updater;
}

void QtIfwPlgTest::testUpdateInstall()
{
	controller->setVersion(QVersionNumber(1, 1, 0));
	controller->createRepository();

	// check for updates to make shure the updates is informed
	auto updater = Updater::createUpdater(QStringLiteral("qtifw"), {
											  {QStringLiteral("path"), controller->maintenanceToolPath() + QStringLiteral("/maintenancetool")}
										  }, this);
	QVERIFY(updater);

	QSignalSpy checkSpy(updater, &Updater::checkUpdatesDone);
	QVERIFY(checkSpy.isValid());
	QSignalSpy exitSpy(updater, &Updater::runOnExitChanged);
	QVERIFY(exitSpy.isValid());

	updater->checkForUpdates();
	QVERIFY(checkSpy.wait(300000));
	QCOMPARE(checkSpy.size(), 1);
	QCOMPARE(checkSpy.takeFirst()[0].value<Updater::Result>(), Updater::Result::NewUpdates);

	// install the update
	QVERIFY(updater->runUpdater(true));
	QCOMPARE(updater->willRunOnExit(), true);
	QCOMPARE(exitSpy.size(), 1);
	QCOMPARE(exitSpy.takeFirst()[0].toBool(), true);

	updater->cancelExitRun();
	QCOMPARE(updater->willRunOnExit(), false);
	QCOMPARE(exitSpy.size(), 1);
	QCOMPARE(exitSpy.takeFirst()[0].toBool(), false);

	// TODO use UpdateInstaller if possible...
//	if (!updater->backend()->features().testFlag(UpdaterBackend::Feature::ParallelInstall))
//		QEXPECT_FAIL("", "Backend does not support parallel update runs on this platform", Abort);
//	QVERIFY(updater->runUpdater(false));

	delete updater;
}

QTEST_GUILESS_MAIN(QtIfwPlgTest)

#include "tst_qtifwplg.moc"
