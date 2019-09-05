#include <QtTest>
#include <QtAutoUpdaterCore>
#include <QtAutoUpdaterCore/private/simplescheduler_p.h>
using namespace QtAutoUpdater;
using namespace std::chrono;
using namespace std::chrono_literals;

class SchedulerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testSimpleScheduling();
	void testCancelScheduling();
	void testLoopedScheduling();
	void testTimepointScheduling();

private:
	using sptr = QScopedPointer<SimpleScheduler, QScopedPointerDeleteLater>;
};

void SchedulerTest::testSimpleScheduling()
{
	sptr scheduler {new SimpleScheduler{this}};
	QSignalSpy triggerSpy{scheduler.data(), &SimpleScheduler::scheduleTriggered};

	QVariant data = 0;
	scheduler->startSchedule(3s, false, data);
	QVERIFY(!triggerSpy.wait(1990));
	QVERIFY(triggerSpy.wait(2020));
	QCOMPARE(triggerSpy.size(), 1);
	QCOMPARE(triggerSpy.takeFirst()[0], data);
}

void SchedulerTest::testCancelScheduling()
{
	sptr scheduler {new SimpleScheduler{this}};
	QSignalSpy triggerSpy{scheduler.data(), &SimpleScheduler::scheduleTriggered};

	QVariant data = 1;
	auto id = scheduler->startSchedule(2s, false, data);
	scheduler->cancelSchedule(id);
	QVERIFY(!triggerSpy.wait(3010));
}

void SchedulerTest::testLoopedScheduling()
{
	sptr scheduler {new SimpleScheduler{this}};
	QSignalSpy triggerSpy{scheduler.data(), &SimpleScheduler::scheduleTriggered};

	QVariant data = 2;
	auto id = scheduler->startSchedule(1s, true, data);
	for (auto i = 0; i < 4; ++i)
		QVERIFY(triggerSpy.wait(2010));
	scheduler->cancelSchedule(id);
	QVERIFY(!triggerSpy.wait(2010));
	QCOMPARE(triggerSpy.size(), 4);
	QCOMPARE(triggerSpy[0][0], data);
	QCOMPARE(triggerSpy[1][0], data);
	QCOMPARE(triggerSpy[2][0], data);
	QCOMPARE(triggerSpy[3][0], data);
	triggerSpy.clear();
}

void SchedulerTest::testTimepointScheduling()
{
	sptr scheduler {new SimpleScheduler{this}};
	QSignalSpy triggerSpy{scheduler.data(), &SimpleScheduler::scheduleTriggered};

	QVariant data = 3;
	scheduler->startSchedule(QDateTime::currentDateTime().addSecs(3), data);
	QVERIFY(!triggerSpy.wait(1990));
	QVERIFY(triggerSpy.wait(2020));
	QCOMPARE(triggerSpy.size(), 1);
	QCOMPARE(triggerSpy.takeFirst()[0], data);
}

QTEST_MAIN(SchedulerTest)

#include "tst_scheduler.moc"
