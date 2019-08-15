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
	QSignalSpy stateSpy(updater, &Updater::stateChanged);
	QVERIFY(stateSpy.isValid());
	QSignalSpy updateInfoSpy(updater, &Updater::updateInfoChanged);
	QVERIFY(updateInfoSpy.isValid());

	//start the check updates
	QCOMPARE(updater->state(), Updater::State::NoUpdates);
	QVERIFY(!updater->isRunning());
	updater->checkForUpdates();

	//runnig should have changed to true
	QCOMPARE(updater->state(), Updater::State::Checking);
	QVERIFY(updater->isRunning());
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Checking);
	QVERIFY(updateInfoSpy.takeFirst()[0].value<QList<UpdateInfo>>().isEmpty());

	//wait max 5 min for the process to finish
	QVERIFY(checkSpy.wait(300000));

	//check if the finished signal is without error
	QCOMPARE(checkSpy.size(), 1);
	QCOMPARE(checkSpy.takeFirst()[0].value<Updater::State>(), hasUpdates ? Updater::State::NewUpdates : Updater::State::NoUpdates);
	QCOMPARE(updater->updateInfo(), updates);
	if(hasUpdates) {
		QCOMPARE(updateInfoSpy.size(), 1);
		QCOMPARE(updateInfoSpy.takeFirst()[0].value<QList<UpdateInfo>>(), updates);
	}

	//runnig should have changed to false
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), hasUpdates ? Updater::State::NewUpdates : Updater::State::NoUpdates);
	QVERIFY(!updater->isRunning());

	//verifiy all signalspies are empty
	QVERIFY(checkSpy.isEmpty());
	QVERIFY(stateSpy.isEmpty());
	QVERIFY(updateInfoSpy.isEmpty());

	//-----------schedule mechanism---------------

	int kId = updater->scheduleUpdate(1, true);//every 1 minute
	QVERIFY(kId);
	updater->cancelScheduledUpdate(kId);

	kId = updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(5));
	QVERIFY(updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(2)));
	updater->cancelScheduledUpdate(kId);

	//wait for the update to start
	QVERIFY(stateSpy.wait(2000 + TEST_DELAY));
	//should be running
	QVERIFY(stateSpy.size() > 0);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Checking);
	//wait for it to finish if not running
	if(stateSpy.isEmpty())
		QVERIFY(stateSpy.wait(120000));
	//should have stopped
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), hasUpdates ? Updater::State::NewUpdates : Updater::State::NoUpdates);

	//wait for the canceled one (max 5 secs)
	QVERIFY(!stateSpy.wait(5000 + TEST_DELAY));

	//verifiy the runningSpy is empty
	QVERIFY(stateSpy.isEmpty());
	//clear the rest
	checkSpy.clear();
	updateInfoSpy.clear();

	delete updater;
}

void QtIfwPlgTest::testUpdateInstall()
{
	controller->setVersion(QVersionNumber(1, 1, 0));
	controller->createRepository();

	// check for updates to make shure the updater is informed
	auto updater = Updater::createUpdater(QStringLiteral("qtifw"), {
											  {QStringLiteral("path"), controller->maintenanceToolPath() + QStringLiteral("/maintenancetool")},
											  {QStringLiteral("silent"), true}
										  }, this);
	QVERIFY(updater);

	QSignalSpy checkSpy(updater, &Updater::checkUpdatesDone);
	QVERIFY(checkSpy.isValid());
	updater->checkForUpdates();
	QVERIFY(checkSpy.wait(300000));
	QCOMPARE(checkSpy.size(), 1);
	QCOMPARE(checkSpy.takeFirst()[0].value<Updater::State>(), Updater::State::NewUpdates);

	// check if exit run is correctly recognized
	QSignalSpy exitSpy(updater, &Updater::runOnExitChanged);
	QVERIFY(exitSpy.isValid());
	QVERIFY(updater->runUpdater(true));
	QCOMPARE(updater->willRunOnExit(), true);
	QCOMPARE(exitSpy.size(), 1);
	QCOMPARE(exitSpy.takeFirst()[0].toBool(), true);

	updater->cancelExitRun();
	QCOMPARE(updater->willRunOnExit(), false);
	QCOMPARE(exitSpy.size(), 1);
	QCOMPARE(exitSpy.takeFirst()[0].toBool(), false);

	// install the update
	QSignalSpy stateSpy(updater, &Updater::stateChanged);
	QVERIFY(stateSpy.isValid());
	QSignalSpy installSpy(updater, &Updater::installDone);
	QVERIFY(installSpy.isValid());
	if (!updater->backend()->features().testFlag(UpdaterBackend::Feature::ParallelInstall))
		QEXPECT_FAIL("", "Backend does not support parallel update runs on this platform", Abort);
	QVERIFY(updater->runUpdater(false));

	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Installing);
	QVERIFY(stateSpy.wait(300000));
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::NoUpdates);
	QCOMPARE(installSpy.size(), 1);
	QCOMPARE(installSpy.takeFirst()[0].toBool(), true);

	// check for updates once again for update to make shure there are none
	checkSpy.clear();
	updater->checkForUpdates();
	QVERIFY(checkSpy.wait(300000));
	QCOMPARE(checkSpy.size(), 1);
	QCOMPARE(checkSpy.takeFirst()[0].value<Updater::State>(), Updater::State::NoUpdates);

	delete updater;
}

QTEST_GUILESS_MAIN(QtIfwPlgTest)

#include "tst_qtifwplg.moc"
