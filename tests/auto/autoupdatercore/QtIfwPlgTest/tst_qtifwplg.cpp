#include <updater.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QVector>
#include <functional>
#include "installercontroller.h"
using namespace QtAutoUpdater;

#define TEST_DELAY 1000

#include <QtTest>

class QtIfwPlgTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void testUpdateCheck_data();
	void testUpdateCheck();

private:
	Updater *updater;

	InstallerController *controller;
	QSignalSpy *checkSpy;
	QSignalSpy *runningSpy;
	QSignalSpy *updateInfoSpy;
};

void QtIfwPlgTest::initTestCase()
{
	controller = new InstallerController(this);
	controller->createRepository();
	controller->createInstaller();
	controller->installLocal();
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

	updater = Updater::createQtIfwUpdater(controller->maintenanceToolPath() + QStringLiteral("/maintenancetool"), false, this);
	QVERIFY(updater);
	checkSpy = new QSignalSpy(updater, &Updater::checkUpdatesDone);
	QVERIFY(checkSpy->isValid());
	runningSpy = new QSignalSpy(updater, &Updater::runningChanged);
	QVERIFY(runningSpy->isValid());
	updateInfoSpy = new QSignalSpy(updater, &Updater::updateInfoChanged);
	QVERIFY(updateInfoSpy->isValid());

	//start the check updates
	QVERIFY(!updater->isRunning());
	updater->checkForUpdates();

	//runnig should have changed to true
	QCOMPARE(runningSpy->size(), 1);
	QVERIFY(runningSpy->takeFirst()[0].toBool());
	QVERIFY(updater->isRunning());
	QVERIFY(updateInfoSpy->takeFirst()[0].value<QList<UpdateInfo>>().isEmpty());

	//wait max 5 min for the process to finish
	QVERIFY(checkSpy->wait(300000));

	//check if the finished signal is without error
	QCOMPARE(checkSpy->size(), 1);
	QVariantList varList = checkSpy->takeFirst();
	QCOMPARE(varList[0].value<Updater::Result>(), hasUpdates ? Updater::Result::NewUpdates : Updater::Result::NoUpdates);
	QCOMPARE(updater->updateInfo(), updates);
	if(hasUpdates) {
		QCOMPARE(updateInfoSpy->size(), 1);
		QCOMPARE(updateInfoSpy->takeFirst()[0].value<QList<UpdateInfo>>(), updates);
	}

	//runnig should have changed to false
	QCOMPARE(runningSpy->size(), 1);
	QVERIFY(!runningSpy->takeFirst()[0].toBool());
	QVERIFY(!updater->isRunning());

	//verifiy all signalspies are empty
	QVERIFY(checkSpy->isEmpty());
	QVERIFY(runningSpy->isEmpty());
	QVERIFY(updateInfoSpy->isEmpty());

	//-----------schedule mechanism---------------

	int kId = updater->scheduleUpdate(1, true);//every 1 minute
	QVERIFY(kId);
	updater->cancelScheduledUpdate(kId);

	kId = updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(5));
	QVERIFY(updater->scheduleUpdate(QDateTime::currentDateTime().addSecs(2)));
	updater->cancelScheduledUpdate(kId);

	//wait for the update to start
	QVERIFY(runningSpy->wait(2000 + TEST_DELAY));
	//should be running
	QVERIFY(runningSpy->size() > 0);
	QVERIFY(runningSpy->takeFirst()[0].toBool());
	//wait for it to finish if not running
	if(runningSpy->isEmpty())
		QVERIFY(runningSpy->wait(120000));
	//should have stopped
	QCOMPARE(runningSpy->size(), 1);
	QVERIFY(!runningSpy->takeFirst()[0].toBool());

	//wait for the canceled one (max 5 secs)
	QVERIFY(!runningSpy->wait(5000 + TEST_DELAY));

	//verifiy the runningSpy is empty
	QVERIFY(runningSpy->isEmpty());
	//clear the rest
	checkSpy->clear();
	updateInfoSpy->clear();

	delete updateInfoSpy;
	delete runningSpy;
	delete checkSpy;
	delete updater;
}

QTEST_GUILESS_MAIN(QtIfwPlgTest)

#include "tst_qtifwplg.moc"
