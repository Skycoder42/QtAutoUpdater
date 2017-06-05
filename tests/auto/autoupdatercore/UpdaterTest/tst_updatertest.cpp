#include <updater.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QVector>
#include <functional>
#include "installercontroller.h"
using namespace QtAutoUpdater;

#define TEST_DELAY 1000

//define before QtTest include because of macos
inline bool operator==(const QtAutoUpdater::Updater::UpdateInfo &a, const QtAutoUpdater::Updater::UpdateInfo &b)
{
	return (a.name == b.name &&
			a.version == b.version &&
			a.size == b.size);
}

#include <QtTest>

class UpdaterTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void testUpdaterInitState();

	void testUpdateCheck_data();
	void testUpdateCheck();

private:
	Updater *updater;

	InstallerController *controller;
	QSignalSpy *checkSpy;
	QSignalSpy *runningSpy;
	QSignalSpy *updateInfoSpy;
};

void UpdaterTest::initTestCase()
{
	controller = new InstallerController(this);
	controller->createRepository();
	controller->createInstaller();
	controller->installLocal();
	controller->setVersion({1, 1, 0});
	controller->createRepository();
}

void UpdaterTest::testUpdaterInitState()
{
	updater = new Updater(this);

	//error state
	QVERIFY(updater->exitedNormally());
	QCOMPARE(updater->errorCode(), EXIT_SUCCESS);
	QVERIFY(updater->errorLog().isEmpty());

	//properties
#if defined(Q_OS_WIN32)
	QCOMPARE(updater->maintenanceToolPath(), QStringLiteral("./maintenancetool.exe"));
#elif defined(Q_OS_OSX)
	QCOMPARE(updater->maintenanceToolPath(), QStringLiteral("../../maintenancetool.app/Contents/MacOS/maintenancetool"));
#elif defined(Q_OS_UNIX)
	QCOMPARE(updater->maintenanceToolPath(), QStringLiteral("./maintenancetool"));
#endif
	QCOMPARE(updater->isRunning(), false);
	QVERIFY(updater->updateInfo().isEmpty());

	delete updater;
}

void UpdaterTest::testUpdateCheck_data()
{
	QTest::addColumn<QString>("toolPath");
	QTest::addColumn<bool>("hasUpdates");
	QTest::addColumn<QList<Updater::UpdateInfo>>("updates");

	QList<Updater::UpdateInfo> updates;
	updates += {"QtAutoUpdaterTestInstaller", QVersionNumber::fromString("1.1.0"), 45ull};
	QString path = controller->maintenanceToolPath();
	QTest::newRow("QtAutoUpdaterTestInstaller") << path + "/maintenancetool"
												<< true
												<< updates;

	updates.clear();

#ifdef Q_OS_WIN
	path = "C:/Qt";
#else
	path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Qt";
#endif
	QTest::newRow("Qt") << path + "/MaintenanceTool"
						<< false
						<< updates;
}

void UpdaterTest::testUpdateCheck()
{
	QFETCH(QString, toolPath);
	QFETCH(bool, hasUpdates);
	QFETCH(QList<Updater::UpdateInfo>, updates);

	updater = new Updater(toolPath, this);
	QVERIFY(updater);
	checkSpy = new QSignalSpy(updater, &Updater::checkUpdatesDone);
	QVERIFY(checkSpy->isValid());
	runningSpy = new QSignalSpy(updater, &Updater::runningChanged);
	QVERIFY(runningSpy->isValid());
	updateInfoSpy = new QSignalSpy(updater, &Updater::updateInfoChanged);
	QVERIFY(updateInfoSpy->isValid());

	//start the check updates
	QVERIFY(!updater->isRunning());
	QVERIFY(updater->checkForUpdates());

	//runnig should have changed to true
	QCOMPARE(runningSpy->size(), 1);
	QVERIFY(runningSpy->takeFirst()[0].toBool());
	QVERIFY(updater->isRunning());
	QVERIFY(updateInfoSpy->takeFirst()[0].value<QList<Updater::UpdateInfo>>().isEmpty());

	//wait max 2 min for the process to finish
	QVERIFY(checkSpy->wait(120000));

	//show error log before continuing checking
	QByteArray log = updater->errorLog();
	if(!log.isEmpty())
		qWarning() << "Error log:" << log;

	//check if the finished signal is without error
	QCOMPARE(checkSpy->size(), 1);
	QVariantList varList = checkSpy->takeFirst();
	QVERIFY(updater->exitedNormally());
	QCOMPARE(updater->errorCode(), hasUpdates ? EXIT_SUCCESS : EXIT_FAILURE);
	QCOMPARE(varList[1].toBool(), false);//no errors please

	//verifiy the "hasUpdates" and "updates" are as expected
	QCOMPARE(varList[0].toBool(), hasUpdates);
	QCOMPARE(updater->updateInfo(), updates);
	if(hasUpdates) {
		QCOMPARE(updateInfoSpy->size(), 1);
		QCOMPARE(updateInfoSpy->takeFirst()[0].value<QList<Updater::UpdateInfo>>(), updates);
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

QTEST_GUILESS_MAIN(UpdaterTest)

#include "tst_updatertest.moc"
