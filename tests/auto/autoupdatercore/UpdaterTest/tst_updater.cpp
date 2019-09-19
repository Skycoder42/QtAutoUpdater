#include <QtTest>
#include <QtAutoUpdaterCore>
#include <QtAutoUpdaterCore/private/updater_p.h>
using namespace QtAutoUpdater;
using namespace std::chrono;
using namespace std::chrono_literals;

Q_DECLARE_METATYPE(seconds)
Q_DECLARE_METATYPE(UpdaterPrivate::InstallerType)
Q_DECLARE_METATYPE(Updater::InstallModeFlag)

class UpdaterTest : public QObject
{
	Q_OBJECT

public:
	enum class ListType {
		String,
		Path,
		Argument
	};
	Q_ENUM(ListType)

private Q_SLOTS:
	void initTestCase();

	void testInitFail();
	void testListConversion_data();
	void testListConversion();
	void testUpdateCheck_data();
	void testUpdateCheck();
	void testAbort_data();
	void testAbort();
	void testSchedule_data();
	void testSchedule();
	void testModeMapping_data();
	void testModeMapping();
	void testTriggerUpdates_data();
	void testTriggerUpdates();

private:
	using sptr = QScopedPointer<Updater, QScopedPointerDeleteLater>;

	void parametrize(QVariantMap &params, const QList<UpdateInfo> &updates);
};

void UpdaterTest::initTestCase()
{
	qRegisterMetaType<seconds>();
	qRegisterMetaType<UpdateInstaller*>();
	qRegisterMetaType<UpdaterPrivate::InstallerType>();
	qRegisterMetaType<Updater::InstallModeFlag>();
}

void UpdaterTest::testInitFail()
{
	QVariantMap config {
		{QStringLiteral("allowInit"), false}
	};
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(!updater);
}

void UpdaterTest::testListConversion_data()
{
	QTest::addColumn<ListType>("type");
	QTest::addColumn<QVariant>("data");
	QTest::addColumn<QStringList>("result");

	QTest::newRow("string.stringlist") << ListType::String
									   << QVariant{QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
									   << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("string.variantlist") << ListType::String
										<< QVariant{QVariantList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
										<< QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("string.standard") << ListType::String
									 << QVariant{QStringLiteral("a,\"b,'c\",d',e")}
									 << QStringList{
											QStringLiteral("a"),
											QStringLiteral("\"b"),
											QStringLiteral("'c\""),
											QStringLiteral("d'"),
											QStringLiteral("e")
										};
	QTest::newRow("string.single") << ListType::String
								   << QVariant{QStringLiteral("a b c")}
								   << QStringList{QStringLiteral("a b c")};
	QTest::newRow("string.empty") << ListType::String
								  << QVariant{QStringLiteral("")}
								  << QStringList{};

	QTest::newRow("path.stringlist") << ListType::Path
									 << QVariant{QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
									 << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("path.variantlist") << ListType::Path
									  << QVariant{QVariantList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
									  << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("path.standard") << ListType::Path
#ifdef Q_OS_WIN
								   << QVariant{QStringLiteral("a;b,c;d:e;f g")}
								   << QStringList{QStringLiteral("a"), QStringLiteral("b,c"), QStringLiteral("d:e"), QStringLiteral("f g")};
#else
								   << QVariant{QStringLiteral("a:b,c:d;e:f g")}
								   << QStringList{
											QStringLiteral("a"),
											QStringLiteral("b,c"),
											QStringLiteral("d;e"),
											QStringLiteral("f g")
										};
#endif
	QTest::newRow("path.single") << ListType::Path
								 << QVariant{QStringLiteral("a b c")}
								 << QStringList{QStringLiteral("a b c")};
	QTest::newRow("path.empty") << ListType::Path
								<< QVariant{QStringLiteral("")}
								<< QStringList{};

	QTest::newRow("args.stringlist") << ListType::Argument
									 << QVariant{QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
									 << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("args.variantlist") << ListType::Argument
									  << QVariant{QVariantList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}}
									  << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("args.standard") << ListType::Argument
								   << QVariant{QStringLiteral("a b c")}
								   << QStringList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
	QTest::newRow("args.single") << ListType::Argument
								 << QVariant{QStringLiteral("a")}
								 << QStringList{QStringLiteral("a")};
	QTest::newRow("args.empty") << ListType::Argument
								<< QVariant{QStringLiteral("")}
								<< QStringList{};
	QTest::newRow("args.complex") << ListType::Argument
								  << QVariant{QStringLiteral("a b \"c d\" 'e f' g\" 'h' \"i j' \"k\" 'l m\\' \\'n o\\\" \\\"p q\\ r 's")}
								  << QStringList {
											QStringLiteral("a"),
											QStringLiteral("b"),
											QStringLiteral("c d"),
											QStringLiteral("e f"),
											QStringLiteral("g 'h' i"),
											QStringLiteral("j \"k\" l"),
											QStringLiteral("m'"),
											QStringLiteral("'n"),
											QStringLiteral("o\""),
											QStringLiteral("\"p"),
											QStringLiteral("q r"),
											QStringLiteral("s")
										};
}

void UpdaterTest::testListConversion()
{
	QFETCH(ListType, type);
	QFETCH(QVariant, data);
	QFETCH(QStringList, result);

	switch (type) {
	case ListType::String:
		QCOMPARE(UpdaterBackend::readStringList(data), result);
		break;
	case ListType::Path:
		QCOMPARE(ProcessBackend::readPathList(data), result);
		break;
	case ListType::Argument:
		QCOMPARE(ProcessBackend::readArgumentList(data), result);
		break;
	}
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
									{0, QStringLiteral("update_0"), QVersionNumber{1,1,2}},
									{1, QStringLiteral("update_1"), QVersionNumber{1,2,1}},
									{2, QStringLiteral("update_2"), QVersionNumber{2,1,1}}
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
								<< false;
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
								 << false;
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
		{QStringLiteral("cancelDelay"), 100},
		{QStringLiteral("abortLevel"), level},
		{QStringLiteral("hasError"), true},
	};

	// prepare the updater
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	QVERIFY(updater);
	QSignalSpy stateSpy{updater.data(), &Updater::stateChanged};
	QVERIFY(stateSpy.isValid());
	QSignalSpy doneSpy{updater.data(), &Updater::checkUpdatesDone};
	QVERIFY(doneSpy.isValid());

	// check for updates and then cancel them
	updater->checkForUpdates();
	QCOMPARE(stateSpy.size(), 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Checking);
	QVERIFY(!doneSpy.wait(500));
	updater->abortUpdateCheck(delay);
	QVERIFY(stateSpy.size() >= 1);
	QCOMPARE(stateSpy.takeFirst()[0].value<Updater::State>(), Updater::State::Canceling);
	stateSpy.clear();
	if (!immediate) {
		QCOMPARE(doneSpy.size(), 0);
		QVERIFY(doneSpy.wait());
	}
	QCOMPARE(doneSpy.size(), 1);
	QCOMPARE(doneSpy.takeFirst()[0].value<Updater::State>(), aborted ? Updater::State::NoUpdates : Updater::State::Error);
}

void UpdaterTest::testSchedule_data()
{
	QTest::addColumn<seconds>("delay");
	QTest::addColumn<bool>("repeated");
	QTest::addColumn<bool>("cancel");

	QTest::newRow("standard") << 3s
							  << false
							  << false;
	QTest::newRow("canceled") << 3s
							  << false
							  << true;
	QTest::newRow("standard") << 3s
							  << true
							  << false;
}

void UpdaterTest::testSchedule()
{
	QFETCH(seconds, delay);
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
			QVERIFY(!stateSpy.wait(static_cast<int>((delay + 1010ms).count())));
		} else {
			QVERIFY(stateSpy.wait(static_cast<int>((delay + 1010ms).count())));
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

void UpdaterTest::testModeMapping_data()
{
	QTest::addColumn<UpdaterBackend::Features>("features");
	QTest::addColumn<Updater::InstallMode>("mode");
	QTest::addColumn<Updater::InstallScope>("scope");
	QTest::addColumn<UpdaterPrivate::InstallerType>("type");

	QTest::newRow("none.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
											<< Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											<< Updater::InstallScope::PreferInternal
											<< UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
												  << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
												  << Updater::InstallScope::PreferInternal
												  << UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
											<< Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											<< Updater::InstallScope::PreferExternal
											<< UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
												  << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
												  << Updater::InstallScope::PreferExternal
												  << UpdaterPrivate::InstallerType::None;

	QTest::newRow("none.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
										  << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
										  << Updater::InstallScope::PreferInternal
										  << UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
												<< Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												<< Updater::InstallScope::PreferInternal
												<< UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
										  << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
										  << Updater::InstallScope::PreferExternal
										  << UpdaterPrivate::InstallerType::None;
	QTest::newRow("none.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::CheckUpdates}
												<< Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												<< Updater::InstallScope::PreferExternal
												<< UpdaterPrivate::InstallerType::None;

	QTest::newRow("trigger.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
											   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											   << Updater::InstallScope::PreferInternal
											   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("trigger.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
													 << Updater::InstallScope::PreferInternal
													 << UpdaterPrivate::InstallerType::None;
	QTest::newRow("trigger.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
											   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											   << Updater::InstallScope::PreferExternal
											   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("trigger.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
													 << Updater::InstallScope::PreferExternal
													 << UpdaterPrivate::InstallerType::None;

	QTest::newRow("trigger.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
											 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
											 << Updater::InstallScope::PreferInternal
											 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("trigger.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
												   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												   << Updater::InstallScope::PreferInternal
												   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("trigger.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
											 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
											 << Updater::InstallScope::PreferExternal
											 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("trigger.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::TriggerInstall}
												   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												   << Updater::InstallScope::PreferExternal
												   << UpdaterPrivate::InstallerType::OnExit;

	QTest::newRow("triggerParallel.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
													   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
													   << Updater::InstallScope::PreferInternal
													   << UpdaterPrivate::InstallerType::Trigger;
	QTest::newRow("triggerParallel.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
															 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
															 << Updater::InstallScope::PreferInternal
															 << UpdaterPrivate::InstallerType::Trigger;
	QTest::newRow("triggerParallel.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
													   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
													   << Updater::InstallScope::PreferExternal
													   << UpdaterPrivate::InstallerType::Trigger;
	QTest::newRow("triggerParallel.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
															 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
															 << Updater::InstallScope::PreferExternal
															 << UpdaterPrivate::InstallerType::Trigger;

	QTest::newRow("triggerParallel.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
													 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
													 << Updater::InstallScope::PreferInternal
													 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("triggerParallel.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
														   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
														   << Updater::InstallScope::PreferInternal
														   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("triggerParallel.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
													 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
													 << Updater::InstallScope::PreferExternal
													 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("triggerParallel.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::ParallelTrigger}
														   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
														   << Updater::InstallScope::PreferExternal
														   << UpdaterPrivate::InstallerType::OnExit;

	QTest::newRow("perform.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
											   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											   << Updater::InstallScope::PreferInternal
											   << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
													 << Updater::InstallScope::PreferInternal
													 << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
											   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
											   << Updater::InstallScope::PreferExternal
											   << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
													 << Updater::InstallScope::PreferExternal
													 << UpdaterPrivate::InstallerType::Perform;

	QTest::newRow("perform.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
											 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
											 << Updater::InstallScope::PreferInternal
											 << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
												   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												   << Updater::InstallScope::PreferInternal
												   << UpdaterPrivate::InstallerType::None;
	QTest::newRow("perform.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
											 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
											 << Updater::InstallScope::PreferExternal
											 << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall}
												   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
												   << Updater::InstallScope::PreferExternal
												   << UpdaterPrivate::InstallerType::None;

	QTest::newRow("perform-trigger.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
													   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
													   << Updater::InstallScope::PreferInternal
													   << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform-trigger.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
															 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
															 << Updater::InstallScope::PreferInternal
															 << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform-trigger.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
													   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
													   << Updater::InstallScope::PreferExternal
													   << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform-trigger.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
															 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
															 << Updater::InstallScope::PreferExternal
															 << UpdaterPrivate::InstallerType::Perform;

	QTest::newRow("perform-trigger.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
													 << Updater::InstallScope::PreferInternal
													 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-trigger.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
														   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
														   << Updater::InstallScope::PreferInternal
														   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-trigger.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
													 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
													 << Updater::InstallScope::PreferExternal
													 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-trigger.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::TriggerInstall}
														   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
														   << Updater::InstallScope::PreferExternal
														   << UpdaterPrivate::InstallerType::OnExit;

	QTest::newRow("perform-triggerParallel.parallel.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
															   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
															   << Updater::InstallScope::PreferInternal
															   << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform-triggerParallel.parallel.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
																	 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
																	 << Updater::InstallScope::PreferInternal
																	 << UpdaterPrivate::InstallerType::Perform;
	QTest::newRow("perform-triggerParallel.parallel.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
															   << Updater::InstallMode{Updater::InstallModeFlag::Parallel}
															   << Updater::InstallScope::PreferExternal
															   << UpdaterPrivate::InstallerType::Trigger;
	QTest::newRow("perform-triggerParallel.parallel.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
																	 << Updater::InstallMode{Updater::InstallModeFlag::Parallel | Updater::InstallModeFlag::Force}
																	 << Updater::InstallScope::PreferExternal
																	 << UpdaterPrivate::InstallerType::Trigger;

	QTest::newRow("perform-triggerParallel.onExit.internal") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
															 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
															 << Updater::InstallScope::PreferInternal
															 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-triggerParallel.onExit.internal.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
																   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
																   << Updater::InstallScope::PreferInternal
																   << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-triggerParallel.onExit.external") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
															 << Updater::InstallMode{Updater::InstallModeFlag::OnExit}
															 << Updater::InstallScope::PreferExternal
															 << UpdaterPrivate::InstallerType::OnExit;
	QTest::newRow("perform-triggerParallel.onExit.external.force") << UpdaterBackend::Features{UpdaterBackend::Feature::PerformInstall | UpdaterBackend::Feature::ParallelTrigger}
																   << Updater::InstallMode{Updater::InstallModeFlag::OnExit | Updater::InstallModeFlag::Force}
																   << Updater::InstallScope::PreferExternal
																   << UpdaterPrivate::InstallerType::OnExit;
}

void UpdaterTest::testModeMapping()
{
	QFETCH(UpdaterBackend::Features, features);
	QFETCH(Updater::InstallMode, mode);
	QFETCH(Updater::InstallScope, scope);
	QFETCH(UpdaterPrivate::InstallerType, type);

	QCOMPARE(UpdaterPrivate::calcInstallerType(mode, scope, features, QStringLiteral("test")), type);
}

void UpdaterTest::testTriggerUpdates_data()
{
	QTest::addColumn<bool>("canInstall");
	QTest::addColumn<Updater::InstallModeFlag>("mode");
	QTest::addColumn<Updater::InstallScope>("scope");

	QTest::addColumn<bool>("works");
	QTest::addColumn<bool>("exitRun");
	QTest::addColumn<bool>("signaled");
	QTest::addColumn<bool>("success");

	QTest::newRow("onExit") << true
							<< Updater::InstallModeFlag::OnExit
							<< Updater::InstallScope::PreferExternal
							<< true
							<< true
							<< false
							<< true;
	QTest::newRow("trigger") << true
							 << Updater::InstallModeFlag::Parallel
							 << Updater::InstallScope::PreferExternal
							 << true
							 << false
							 << false
							 << true;
	QTest::newRow("perform") << true
							 << Updater::InstallModeFlag::Parallel
							 << Updater::InstallScope::PreferInternal
							 << true
							 << false
							 << true
							 << true;
	QTest::newRow("none") << false
						  << Updater::InstallModeFlag::Parallel
						  << Updater::InstallScope::PreferInternal
						  << false
						  << false
						  << false
						  << false;

	QTest::newRow("trigger.failed") << true
									<< Updater::InstallModeFlag::Parallel
									<< Updater::InstallScope::PreferExternal
									<< true
									<< false
									<< false
									<< false;
	QTest::newRow("perform.failed") << true
									<< Updater::InstallModeFlag::Parallel
									<< Updater::InstallScope::PreferInternal
									<< true
									<< false
									<< true
									<< false;
}

void UpdaterTest::testTriggerUpdates()
{
	QFETCH(bool, canInstall);
	QFETCH(Updater::InstallModeFlag, mode);
	QFETCH(Updater::InstallScope, scope);
	QFETCH(bool, works);
	QFETCH(bool, exitRun);
	QFETCH(bool, signaled);
	QFETCH(bool, success);

	// prepare the configuration
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(canInstall ?
														  (UpdaterBackend::Feature::ParallelTrigger | UpdaterBackend::Feature::PerformInstall) :
														  UpdaterBackend::Feature::CheckUpdates)},
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
	const auto ok = updater->runUpdater(mode, scope);
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
			installer->setComponents({UpdateInfo{0, {}, {}}}); // set a component so the installer can actually operate
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
		params.insert(QStringLiteral("updates/%1/id").arg(i), info.identifier());
		params.insert(QStringLiteral("updates/%1/name").arg(i), info.name());
		params.insert(QStringLiteral("updates/%1/version").arg(i), info.version().toString());
		params.insert(QStringLiteral("updates/%1/data").arg(i), QVariant{info.data().keys()});
		for (auto it = info.data().constBegin(); it != info.data().constEnd(); ++it)
			params.insert(QStringLiteral("updates/%1/%2").arg(i).arg(it.key()), it.value());
	}
}

QTEST_MAIN(UpdaterTest)

#include "tst_updater.moc"
