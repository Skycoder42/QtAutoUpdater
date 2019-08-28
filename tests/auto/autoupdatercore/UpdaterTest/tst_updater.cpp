#include <QtTest>
#include <QtAutoUpdaterCore>
using namespace QtAutoUpdater;
using namespace std::chrono;

class UpdaterTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void testInitFail();
	void testUpdateCheck_data();
	void testUpdateCheck();
	void testAbort_data();
	void testAbort();
	void testSchedule_data();
	void testSchedule();
	void testTriggerUpdates_data();
	void testTriggerUpdates();

private:
	using sptr = QScopedPointer<Updater, QScopedPointerDeleteLater>;

	void parametrize(QVariantMap &params, const QList<UpdateInfo> &updates);
};

void UpdaterTest::initTestCase()
{
	qRegisterMetaType<UpdateInstaller*>();
}

void UpdaterTest::testInitFail()
{
	QVariantMap config {
		{QStringLiteral("allowInit"), false}
	};
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(!updater);
}

void UpdaterTest::testUpdateCheck_data()
{
	QTest::addColumn<bool>("progress");
	QTest::addColumn<bool>("success");
	QTest::addColumn<QList<UpdateInfo>>("updates");

	QTest::newRow("noUpdates") << false
							   << true
							   << QList<UpdateInfo>{};
	QTest::newRow("updates") << false
							 << true
							 << QList<UpdateInfo>{
									{QStringLiteral("update_0"), QVersionNumber{1,1,2}, 1000ull, 0},
									{QStringLiteral("update_1"), QVersionNumber{1,2,1}, 1000000ull, 1},
									{QStringLiteral("update_2"), QVersionNumber{2,1,1}, 1000000000ull, 1}
								};
	QTest::newRow("error") << false
						   << false
						   << QList<UpdateInfo>{};
	QTest::newRow("progress") << true
							  << true
							  << QList<UpdateInfo>{};
}

void UpdaterTest::testUpdateCheck()
{
	QFETCH(bool, progress);
	QFETCH(bool, success);
	QFETCH(QList<UpdateInfo>, updates);

	// prepare the arguments
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(progress ? UpdaterBackend::Feature::CheckProgress : UpdaterBackend::Feature::CheckUpdates)},
		{QStringLiteral("delay"), 100},
		{QStringLiteral("status"), QStringLiteral("test_status")},
		{QStringLiteral("hasError"), !success},
	};
	parametrize(config, updates);

	// prepare the updater
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(updater);
	QSignalSpy stateSpy{updater.data(), &Updater::stateChanged};
	QVERIFY(stateSpy.isValid());
	QSignalSpy infoSpy{updater.data(), &Updater::updateInfoChanged};
	QVERIFY(infoSpy.isValid());
	QSignalSpy progressSpy{updater.data(), &Updater::progressChanged};
	QVERIFY(progressSpy.isValid());
	QSignalSpy doneSpy{updater.data(), &Updater::checkUpdatesDone};
	QVERIFY(doneSpy.isValid());

	// verify the initial state
	QCOMPARE(updater->backend()->features().testFlag(UpdaterBackend::Feature::CheckProgress), progress);
	QCOMPARE(updater->state(), Updater::State::NoUpdates);
	QCOMPARE(updater->isRunning(), false);
	QVERIFY(updater->updateInfo().isEmpty());

	// check for updates
	updater->checkForUpdates();
	QCOMPARE(updater->state(), Updater::State::Checking);
	QCOMPARE(updater->isRunning(), true);
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Checking);

	// track the progress
	if (progress) {
		QCOMPARE(progressSpy.size(), 2);
		QCOMPARE(progressSpy.last()[0].value<double>(), 0.0);
		QCOMPARE(progressSpy.last()[1].value<QString>(), QStringLiteral("test_status"));
		progressSpy.clear();
		for (auto i = 1; i <= 10; ++i) {
			QVERIFY(progressSpy.wait());
			QCOMPARE(progressSpy.size(), 1);
			QCOMPARE(progressSpy.takeFirst()[0].value<double>(), i / 10.0);
		}
	} else
		QCOMPARE(progressSpy.size(), 1);

	// wait until finished
	if (doneSpy.isEmpty())
		QVERIFY(doneSpy.wait());
	const auto resState = success ?
							  (updates.isEmpty() ? Updater::State::NoUpdates : Updater::State::NewUpdates) :
							  Updater::State::Error;
	QCOMPARE(updater->isRunning(), false);
	QCOMPARE(updater->state(), resState);
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), resState);
	QCOMPARE(doneSpy.size(), 1);
	QCOMPARE(doneSpy.takeFirst()[0].value<Updater::State>(), resState);
	if (success) {
		QVERIFY(!infoSpy.isEmpty());
		QCOMPARE(infoSpy.last()[0].value<QList<UpdateInfo>>(), updates);
		infoSpy.clear();
	}
}

void UpdaterTest::testAbort_data()
{
	QTest::addColumn<int>("level");
	QTest::addColumn<int>("delay");
	QTest::addColumn<bool>("aborted");
	QTest::addColumn<bool>("immediate");

	QTest::newRow("soft.allow") << 0
								<< -1
								<< true
								<< true;
	QTest::newRow("soft.force") << 1
								<< -1
								<< false
								<< false;
	QTest::newRow("soft.deny") << 2
							   << -1
							   << false
							   << false;
	QTest::newRow("hard.allow") << 0
								<< 0
								<< true
								<< true;
	QTest::newRow("hard.force") << 1
								<< 0
								<< true
								<< true;
	QTest::newRow("hard.deny") << 2
							   << 0
							   << false
							   << false;
	QTest::newRow("mixed.allow") << 0
								 << 300
								 << true
								 << true;
	QTest::newRow("mixed.force") << 1
								 << 300
								 << true
								 << false;
	QTest::newRow("mixed.deny") << 2
								<< 300
								<< false
								<< false;
}

void UpdaterTest::testAbort()
{
	QFETCH(int, level);
	QFETCH(int, delay);
	QFETCH(bool, aborted);
	QFETCH(bool, immediate);

	// prepare the arguments
	QVariantMap config {
		{QStringLiteral("delay"), 100},
		{QStringLiteral("abortLevel"), level},
		{QStringLiteral("hasError"), true},
	};

	// prepare the updater
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(updater);
	QSignalSpy doneSpy{updater.data(), &Updater::checkUpdatesDone};
	QVERIFY(doneSpy.isValid());

	// check for updates and then cancel them
	updater->checkForUpdates();
	QVERIFY(!doneSpy.wait(500));
	updater->abortUpdateCheck(delay);
	if (!immediate) {
		QCOMPARE(doneSpy.size(), 0);
		QVERIFY(doneSpy.wait());
	}
	QCOMPARE(doneSpy.size(), 1);
	QCOMPARE(doneSpy.takeFirst()[0].value<Updater::State>(), aborted ? Updater::State::NoUpdates : Updater::State::Error);
}

void UpdaterTest::testSchedule_data()
{
	QTest::addColumn<int>("delay");
	QTest::addColumn<bool>("repeated");
	QTest::addColumn<bool>("cancel");

	QTest::newRow("standard") << 3
							  << false
							  << false;
	QTest::newRow("canceled") << 3
							  << false
							  << true;
	QTest::newRow("standard") << 3
							  << true
							  << false;
}

void UpdaterTest::testSchedule()
{
	QFETCH(int, delay);
	QFETCH(bool, repeated);
	QFETCH(bool, cancel);

	// prepare the arguments
	QVariantMap config {
		{QStringLiteral("delay"), 100}
	};

	// prepare the updater
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(updater);
	QSignalSpy stateSpy{updater.data(), &Updater::stateChanged};
	QVERIFY(stateSpy.isValid());

	// schedule the update
	const auto id = updater->scheduleUpdate(delay, repeated);
	QVERIFY(id != 0);
	auto repCnt = repeated ? 2 : 0;
	do {
		if (cancel) {
			updater->cancelScheduledUpdate(id);
			QVERIFY(!stateSpy.wait(delay * 1000 + 1010));
		} else {
			QVERIFY(stateSpy.wait(delay * 1000 + 1010));
			QCOMPARE(stateSpy.size(), 1);
			QCOMPARE(stateSpy[0][0].value<Updater::State>(), Updater::State::Checking);
			updater->abortUpdateCheck(0);
			stateSpy.clear();
			if (repCnt == 1)
				cancel = true;
		}
		QCOMPARE(updater->state(), Updater::State::NoUpdates);
	} while (repCnt-- > 0);
}

void UpdaterTest::testTriggerUpdates_data()
{
	QTest::addColumn<bool>("canTrigger");
	QTest::addColumn<bool>("canParallel");
	QTest::addColumn<bool>("canInstall");
	QTest::addColumn<bool>("forceExit");

	QTest::addColumn<bool>("works");
	QTest::addColumn<bool>("exitRun");
	QTest::addColumn<bool>("signaled");
	QTest::addColumn<bool>("success");

	QTest::newRow("trigger.detached") << true
									  << false
									  << false
									  << false
									  << true
									  << true
									  << false
									  << true;
	QTest::newRow("trigger.detached.forced") << true
											 << false
											 << false
											 << true
											 << true
											 << true
											 << false
											 << true;
	QTest::newRow("trigger.parallel") << true
									  << true
									  << false
									  << false
									  << true
									  << false
									  << false
									  << true;
	QTest::newRow("trigger.parallel.forced") << true
											 << true
											 << false
											 << true
											 << true
											 << true
											 << false
											 << true;
	QTest::newRow("trigger.parallel.failed") << true
											 << true
											 << false
											 << false
											 << true
											 << false
											 << false
											 << false;
	QTest::newRow("perform") << false
							 << false
							 << true
							 << false
							 << true
							 << false
							 << true
							 << true;
	QTest::newRow("perform.forced") << false
									<< false
									<< true
									<< true
									<< false
									<< false
									<< false
									<< false;
	QTest::newRow("perform.failed") << false
									<< false
									<< true
									<< false
									<< true
									<< false
									<< true
									<< false;
}

void UpdaterTest::testTriggerUpdates()
{
	QFETCH(bool, canTrigger);
	QFETCH(bool, canParallel);
	QFETCH(bool, canInstall);
	QFETCH(bool, forceExit);
	QFETCH(bool, works);
	QFETCH(bool, exitRun);
	QFETCH(bool, signaled);
	QFETCH(bool, success);

	// prepare the configuration
	UpdaterBackend::Features features = UpdaterBackend::Feature::CheckUpdates;
	if (canTrigger)
		features |= UpdaterBackend::Feature::TriggerInstall;
	if (canParallel)
		features |= UpdaterBackend::Feature::ParallelInstall;
	if (canInstall)
		features |= UpdaterBackend::Feature::PerformInstall;
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(features)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("updateTime"), 1000},
		{QStringLiteral("updateHasError"), !success},

		{QStringLiteral("installer/delay"), 100},
		{QStringLiteral("installer/success"), success},
	};

	// prepare the updater
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(updater);
	QSignalSpy stateSpy{updater.data(), &Updater::stateChanged};
	QVERIFY(stateSpy.isValid());
	QSignalSpy exitSpy{updater.data(), &Updater::runOnExitChanged};
	QVERIFY(exitSpy.isValid());
	QSignalSpy doneSpy{updater.data(), &Updater::installDone};
	QVERIFY(doneSpy.isValid());
	QSignalSpy showSpy{updater.data(), &Updater::showInstaller};
	QVERIFY(showSpy.isValid());

	// trigger the update installation
	const auto ok = updater->runUpdater(forceExit);
	QCOMPARE(ok, works);
	if (!works)
		return;
	if (exitRun) {
		QCOMPARE(updater->willRunOnExit(), true);
		QCOMPARE(exitSpy.size(), 1);
		QCOMPARE(exitSpy.takeFirst()[0].toBool(), true);

		// cancel the exit run (cannot be tested)
		updater->cancelExitRun();
		QCOMPARE(updater->willRunOnExit(), false);
		QCOMPARE(exitSpy.size(), 1);
		QCOMPARE(exitSpy.takeFirst()[0].toBool(), false);
	} else {
		QCOMPARE(updater->willRunOnExit(), false);
		QCOMPARE(stateSpy.size(), 1);
		QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Installing);
		QCOMPARE(showSpy.size(), signaled ? 1 : 0);
		if (signaled) {
			auto installer = showSpy.takeFirst()[0].value<UpdateInstaller*>();
			QVERIFY(installer);
			installer->setComponents({{{}, {}, 0, 42}}); // set a component so the installer can actually operate
			installer->startInstall();
		}

		// wait for finished
		QVERIFY(doneSpy.wait());
		QCOMPARE(doneSpy.size(), 1);
		QCOMPARE(doneSpy.takeFirst()[0].toBool(), success);
		QCOMPARE(stateSpy.size(), 1);
		QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), success ? Updater::State::NoUpdates : Updater::State::Error);
	}
}

void UpdaterTest::parametrize(QVariantMap &params, const QList<UpdateInfo> &updates)
{
	params.insert(QStringLiteral("updates/size"), updates.size());
	for (auto i = 0; i < updates.size(); ++i) {
		const auto &info = updates[i];
		params.insert(QStringLiteral("updates/%1/name").arg(i), info.name());
		params.insert(QStringLiteral("updates/%1/version").arg(i), info.version().toString());
		params.insert(QStringLiteral("updates/%1/size").arg(i), info.size());
		params.insert(QStringLiteral("updates/%1/id").arg(i), info.identifier());
	}
}

QTEST_MAIN(UpdaterTest)

#include "tst_updater.moc"
