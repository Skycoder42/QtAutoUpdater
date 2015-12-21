#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QProcess>

#include <autoupdater.h>

class UpdaterTest : public QObject
{
	Q_OBJECT

public:
	inline UpdaterTest() {}

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testMaintenanceTool_data();
	void testMaintenanceTool();

	void testUpdaterBasics();

private:
	AutoUpdater *updater;
};

void UpdaterTest::initTestCase()
{
	this->updater = new AutoUpdater(this);
}

void UpdaterTest::cleanupTestCase()
{
	delete this->updater;
}

void UpdaterTest::testMaintenanceTool_data()
{
	QTest::addColumn<QString>("tool");
	QTest::newRow("0") << "C:/Program Files/IcoDroid/maintenancetool.exe";
}

void UpdaterTest::testMaintenanceTool()
{
//	QFETCH(QString, tool);

//	QProcess proc;
//	proc.setProgram(tool);
//	proc.setArguments({QStringLiteral("--checkupdates")});

//	proc.start();
//	proc.waitForFinished(-1);
//	qDebug() << proc.readAllStandardOutput();

//	QVERIFY(proc.exitCode() == 0);
}

void UpdaterTest::testUpdaterBasics()
{
	//normal states
	QCOMPARE(this->updater->getErrorCode(), EXIT_SUCCESS);
	QVERIFY(this->updater->getErrorLog().isEmpty());

	//properties
	QCOMPARE(this->updater->maintenanceToolPath(), QStringLiteral("./maintenancetool.exe"));
	QCOMPARE(this->updater->isRunning(), false);
	QCOMPARE(this->updater->updateArguments(), QStringList("--updater"));
	QCOMPARE(this->updater->runAsAdmin(), false);
	QVERIFY(this->updater->updateInfo().isEmpty());
}

QTEST_GUILESS_MAIN(UpdaterTest)

#include "tst_updatertest.moc"
