#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QSignalSpy>

#include <autoupdater.h>

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
	QCOMPARE(this->updater->updateArguments(), QStringList("--updater"));
	QCOMPARE(this->updater->runAsAdmin(), false);
	QVERIFY(this->updater->updateInfo().isEmpty());
}

void UpdaterTest::testUpdateCheck_data()
{
	QTest::addColumn<QString>("toolPath");
	QTest::addColumn<QStringList>("updateArgs");
	QTest::addColumn<bool>("admin");
	QTest::addColumn<bool>("hasUpdates");
	QTest::addColumn<QList<AutoUpdater::UpdateInfo>>("updates");

	QList<AutoUpdater::UpdateInfo> updates;
	updates += {"IcoDroid", QVersionNumber::fromString("1.0.1"), 52300641ull};
	QTest::newRow("0") << "C:/Program Files/IcoDroid/maintenancetool.exe"
					   << QStringList("--updater")
					   << false
					   << true
					   << updates;
}

void UpdaterTest::testUpdateCheck()
{
	QFETCH(QString, toolPath);
	QFETCH(QStringList, updateArgs);
	QFETCH(bool, admin);
	QFETCH(bool, hasUpdates);
	QFETCH(QList<AutoUpdater::UpdateInfo>, updates);

	QVERIFY(!this->updater->isRunning());

	this->updater->setMaintenanceToolPath(toolPath);
	this->updater->setUpdateArguments(updateArgs);
	this->updater->setRunAsAdmin(admin);
}

QTEST_GUILESS_MAIN(UpdaterTest)

#include "tst_updatertest.moc"
