#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QSignalSpy>

#include <autoupdater.h>

inline bool operator==(const AutoUpdater::UpdateInfo &a, const AutoUpdater::UpdateInfo &b) {
	return (a.name == b.name &&
			a.version == b.version &&
			a.size == b.size);
}

class UpdaterTest : public QObject
{
	Q_OBJECT

public:
	inline UpdaterTest() {}

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testMaintenanceTool();
	void testUpdaterInitState();

	void testUpdateCheck_data();
	void testUpdateCheck();

private:
	AutoUpdater *updater;
	QSignalSpy *checkSpy;
	QSignalSpy *runningSpy;
	QSignalSpy *updateInfoSpy;
};

void UpdaterTest::initTestCase()
{
	this->updater = new AutoUpdater(this);
	QVERIFY(this->updater);
	this->checkSpy = new QSignalSpy(this->updater, &AutoUpdater::checkUpdatesDone);
	QVERIFY(this->checkSpy->isValid());
	this->runningSpy = new QSignalSpy(this->updater, &AutoUpdater::runningChanged);
	QVERIFY(this->runningSpy->isValid());
	this->updateInfoSpy = new QSignalSpy(this->updater, &AutoUpdater::updateInfoChanged);
	QVERIFY(this->updateInfoSpy->isValid());
}

void UpdaterTest::cleanupTestCase()
{
	delete this->updateInfoSpy;
	delete this->runningSpy;
	delete this->checkSpy;
	delete this->updater;
}

void UpdaterTest::testMaintenanceTool()
{
//	QProcess proc;
//	proc.setProgram("C:/Program Files/IcoDroid/maintenancetool.exe");
//	proc.setArguments({QStringLiteral("--checkupdates")});

//	proc.start();
//	proc.waitForFinished(-1);
//	qDebug() << proc.readAllStandardOutput();

//	QVERIFY(proc.exitCode() == 0);
}

void UpdaterTest::testUpdaterInitState()
{
	//error state
	QVERIFY(this->updater->exitedNormally());
	QCOMPARE(this->updater->getErrorCode(), EXIT_SUCCESS);
	QVERIFY(this->updater->getErrorLog().isEmpty());

	//properties
	QCOMPARE(this->updater->maintenanceToolPath(), QStringLiteral("./maintenancetool.exe"));
	QCOMPARE(this->updater->isRunning(), false);
	QVERIFY(this->updater->updateInfo().isEmpty());
}

void UpdaterTest::testUpdateCheck_data()
{
	QTest::addColumn<QString>("toolPath");
	QTest::addColumn<bool>("hasUpdates");
	QTest::addColumn<QList<AutoUpdater::UpdateInfo>>("updates");

	QList<AutoUpdater::UpdateInfo> updates;
	updates += {"IcoDroid", QVersionNumber::fromString("1.0.1"), 52300641ull};
	QTest::newRow("C:/Program Files/IcoDroid") << "C:/Program Files/IcoDroid/maintenancetool.exe"
											   << true
											   << updates;
}

void UpdaterTest::testUpdateCheck()
{
	QFETCH(QString, toolPath);
	QFETCH(bool, hasUpdates);
	QFETCH(QList<AutoUpdater::UpdateInfo>, updates);

	//start the check updates
	QVERIFY(!this->updater->isRunning());
	this->updater->setMaintenanceToolPath(toolPath);
	QVERIFY(this->updater->checkForUpdates());

	//runnig should have changed to true
	QCOMPARE(this->runningSpy->size(), 1);
	QVERIFY(this->runningSpy->takeFirst()[0].toBool());
	QVERIFY(this->updater->isRunning());

	//wait max 1 min for the process to finish
	QVERIFY(this->checkSpy->wait(60000));

	//show error log before continuing checking
	QByteArray log = this->updater->getErrorLog();
	if(!log.isEmpty())
		qWarning() << "Error log:" << log;

	//check if the finished signal is without error
	QCOMPARE(this->checkSpy->size(), 1);
	QVariantList varList = this->checkSpy->takeFirst();
	QVERIFY(this->updater->exitedNormally());
	QCOMPARE(this->updater->getErrorCode(), EXIT_SUCCESS);
	QVERIFY(!varList[1].toBool());

	//verifiy the "hasUpdates" and "updates" are as expected
	QCOMPARE(varList[0].toBool(), hasUpdates);
	QCOMPARE(this->updater->updateInfo(), updates);

	//runnig should have changed to false
	QCOMPARE(this->runningSpy->size(), 1);
	QVERIFY(!this->runningSpy->takeFirst()[0].toBool());
	QVERIFY(!this->updater->isRunning());

	//verifiy all signalspies are empty
	QVERIFY(this->checkSpy->isEmpty());
	QVERIFY(this->runningSpy->isEmpty());
	QVERIFY(this->updateInfoSpy->isEmpty());
}

QTEST_GUILESS_MAIN(UpdaterTest)

#include "tst_updatertest.moc"
