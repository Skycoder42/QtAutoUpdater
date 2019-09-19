#include <QtTest>
#include <QtAutoUpdaterCore>
#include <QtAutoUpdaterCore/private/updateinstaller_p.h>
using namespace QtAutoUpdater;

class UpdateInstallerTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void testComponentSelection();
	void testInstallFlow_data();
	void testInstallFlow();
	void testInstallCancel();
	void testInstallEula_data();
	void testInstallEula();
	void testInstallProgress();

private:
	using sptr = QScopedPointer<Updater, QScopedPointerDeleteLater>;

	void parametrize(QVariantMap &params, const QList<UpdateInfo> &updates);
	void parametrize(QVariantMap &params, const QList<std::pair<QString, bool>> &eulas);
	UpdateInstaller *getInstaller(Updater *updater);
};

void UpdateInstallerTest::initTestCase()
{
	qRegisterMetaType<UpdateInstaller*>();
}

void UpdateInstallerTest::testComponentSelection()
{
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(UpdaterBackend::Feature::PerformInstall)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("installer/features"), static_cast<int>(UpdateInstaller::Feature::SelectComponents)},
	};
	QList<UpdateInfo> updates {
		{0, QStringLiteral("test_1"), {1,1,2}},
		{1, QStringLiteral("test_2"), {1,2,0}},
		{2, QStringLiteral("test_3"), {2,0,0}}
	};
	parametrize(config, updates);

	// trigger updates
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	auto installer = getInstaller(updater.data());
	QVERIFY(installer);

	//verify component state
	QCOMPARE(installer->components().size(), updates.size());
	for (auto i = 0; i < updates.size(); ++i)
		QCOMPARE(installer->components()[i], updates[i]);

	auto model = installer->componentModel();
	QAbstractItemModelTester modelTester {
		model,
		QAbstractItemModelTester::FailureReportingMode::QtTest
	};

	// verify model data
	QCOMPARE(model->rowCount(), updates.size());
	for (auto i = 0; i < updates.size(); ++i) {
		QCOMPARE(model->data(model->index(i, 0), Qt::DisplayRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 0), Qt::ToolTipRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 0), Qt::CheckStateRole).toInt(), Qt::Checked);
		QCOMPARE(model->data(model->index(i, 1), Qt::DisplayRole).toString(), updates[i].version().toString());
		QCOMPARE(model->data(model->index(i, 1), Qt::ToolTipRole).toString(), updates[i].version().toString());

		QCOMPARE(model->data(model->index(i, 0), ComponentModel::NameRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 0), ComponentModel::VersionRole).toString(), updates[i].version().toString());
		QCOMPARE(model->data(model->index(i, 0), ComponentModel::SelectedRole).toBool(), true);
		QCOMPARE(model->data(model->index(i, 0), ComponentModel::UpdateInfoRole).value<UpdateInfo>(), updates[i]);
	}

	// modify model data
	QVERIFY(model->setData(model->index(0, 0), Qt::Unchecked, Qt::CheckStateRole));
	QCOMPARE(model->data(model->index(0, 0), Qt::CheckStateRole).toInt(), Qt::Unchecked);
	QCOMPARE(model->data(model->index(0, 0), ComponentModel::SelectedRole).toBool(), false);
	QVERIFY(model->setData(model->index(2, 0), false, ComponentModel::SelectedRole));
	QCOMPARE(model->data(model->index(2, 0), Qt::CheckStateRole).toInt(), Qt::Unchecked);
	QCOMPARE(model->data(model->index(2, 0), ComponentModel::SelectedRole).toBool(), false);

	// check components return correctly
	QCOMPARE(installer->components().size(), 1);
	QCOMPARE(installer->components().first(), updates[1]);
}

void UpdateInstallerTest::testInstallFlow_data()
{
	QTest::addColumn<bool>("restart");
	QTest::addColumn<bool>("success");

	QTest::newRow("success") << false
							 << true;
	QTest::newRow("success.restart") << true
									 << true;
	QTest::newRow("failure") << false
							 << false;
}

void UpdateInstallerTest::testInstallFlow()
{
	QFETCH(bool, restart);
	QFETCH(bool, success);

	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(UpdaterBackend::Feature::PerformInstall)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("installer/features"), static_cast<int>(UpdateInstaller::Feature::None)},
		{QStringLiteral("installer/delay"), 10},
		{QStringLiteral("installer/restart"), restart},
		{QStringLiteral("installer/success"), success}
	};
	QList<UpdateInfo> updates {
		{0, QStringLiteral("test_1"), {1,1,2}},
		{1, QStringLiteral("test_2"), {1,2,0}},
	};
	parametrize(config, updates);

	// trigger updates
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	auto installer = getInstaller(updater.data());
	QVERIFY(installer);

	QSignalSpy doneSpy{installer, &UpdateInstaller::installSucceeded};
	QVERIFY(doneSpy.isValid());
	QSignalSpy errorSpy{installer, &UpdateInstaller::installFailed};
	QVERIFY(errorSpy.isValid());

	// start install and wait for the result
	installer->startInstall();
	if (success) {
		QVERIFY(doneSpy.wait());
		QCOMPARE(doneSpy.size(), 1);
		QCOMPARE(doneSpy.takeFirst()[0].toBool(), restart);
		QCOMPARE(errorSpy.size(), 0);
		QVERIFY(!errorSpy.wait(1000));
	} else {
		QVERIFY(errorSpy.wait());
		QCOMPARE(errorSpy.size(), 1);
		QCOMPARE(errorSpy.takeFirst()[0].toString(), QStringLiteral("installer/success"));
		QCOMPARE(doneSpy.size(), 0);
		QVERIFY(!doneSpy.wait(1000));
	}
}

void UpdateInstallerTest::testInstallCancel()
{
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(UpdaterBackend::Feature::PerformInstall)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("installer/features"), static_cast<int>(UpdateInstaller::Feature::CanCancel)},
		{QStringLiteral("installer/delay"), 100}
	};
	QList<UpdateInfo> updates {
		{0, QStringLiteral("test_1"), {1,1,2}},
		{1, QStringLiteral("test_2"), {1,2,0}},
	};
	parametrize(config, updates);

	// trigger updates
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	auto installer = getInstaller(updater.data());
	QVERIFY(installer);

	QSignalSpy doneSpy{installer, &UpdateInstaller::installSucceeded};
	QVERIFY(doneSpy.isValid());
	QSignalSpy errorSpy{installer, &UpdateInstaller::installFailed};
	QVERIFY(errorSpy.isValid());

	// start install and then cancel it
	installer->startInstall();
	QVERIFY(!doneSpy.wait(500));
	installer->cancelInstall();
	if (errorSpy.isEmpty())
		QVERIFY(errorSpy.wait());
	QCOMPARE(errorSpy.size(), 1);
	QCOMPARE(errorSpy.takeFirst()[0].toString(), QStringLiteral("canceled"));
	QVERIFY(!doneSpy.wait());
	QCOMPARE(doneSpy.size(), 0);
}

void UpdateInstallerTest::testInstallEula_data()
{
	QTest::addColumn<int>("eulaCnt");
	QTest::addColumn<QList<int>>("requiredIndices");
	QTest::addColumn<bool>("success");

	QTest::newRow("single.optional") << 1
									 << QList<int>{}
									 << true;
	QTest::newRow("single.required.accept") << 1
											<< QList<int>{0}
											<< true;
	QTest::newRow("single.required.reject") << 1
											<< QList<int>{0}
											<< false;
	QTest::newRow("multi.optional") << 3
									<< QList<int>{}
									<< true;
	QTest::newRow("multi.required.single.accept") << 3
												  << QList<int>{1}
												  << true;
	QTest::newRow("multi.required.single.reject") << 3
												  << QList<int>{1}
												  << false;
	QTest::newRow("multi.required.multi.accept") << 3
												 << QList<int>{0, 2}
												 << true;
	QTest::newRow("multi.required.multi.reject") << 3
												 << QList<int>{0, 2}
												 << false;
}

void UpdateInstallerTest::testInstallEula()
{
	QFETCH(int, eulaCnt);
	QFETCH(QList<int>, requiredIndices);
	QFETCH(bool, success);

	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(UpdaterBackend::Feature::PerformInstall)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("installer/features"), static_cast<int>(UpdateInstaller::Feature::None)},
		{QStringLiteral("installer/delay"), 1}
	};
	QList<UpdateInfo> updates {
		{0, QStringLiteral("test_1"), {1,1,2}}
	};
	parametrize(config, updates);

	// setup eulas
	QList<std::pair<QString, bool>> eulas;
	eulas.reserve(eulaCnt);
	for (auto i = 0; i < eulaCnt; ++i)
		eulas.append({QStringLiteral("eula_%1").arg(i), requiredIndices.contains(i)});
	parametrize(config, eulas);

	// trigger updates
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	auto installer = getInstaller(updater.data());
	QVERIFY(installer);

	QSignalSpy eulaSpy{installer, &UpdateInstaller::showEula};
	QVERIFY(eulaSpy.isValid());
	QSignalSpy doneSpy{installer, &UpdateInstaller::installSucceeded};
	QVERIFY(doneSpy.isValid());
	QSignalSpy errorSpy{installer, &UpdateInstaller::installFailed};
	QVERIFY(errorSpy.isValid());

	// start install and verify all eulas
	installer->startInstall();
	QUuid failId;
	for (const auto &eula : qAsConst(eulas)) {
		if (eulaSpy.isEmpty())
			QVERIFY(eulaSpy.wait());
		QCOMPARE(eulaSpy[0][1].toString(), eula.first);
		QCOMPARE(eulaSpy[0][2].toBool(), eula.second);
		if (eula.second) {
			installer->eulaHandled(eulaSpy[0][0].toUuid(), success);
			if (!success && failId.isNull())
				failId = eulaSpy[0][0].toUuid();
		}
		eulaSpy.removeFirst();
	}

	if (success) {
		if (doneSpy.isEmpty())
			QVERIFY(doneSpy.wait());
		QCOMPARE(doneSpy.size(), 1);
	} else {
		if (errorSpy.isEmpty())
			QVERIFY(errorSpy.wait());
		QCOMPARE(errorSpy.size(), 1);
		QCOMPARE(errorSpy.takeFirst()[0].toString(), QStringLiteral("eula/%1").arg(failId.toString()));
	}
}

void UpdateInstallerTest::testInstallProgress()
{
	QVariantMap config {
		{QStringLiteral("features"), static_cast<int>(UpdaterBackend::Feature::PerformInstall)},
		{QStringLiteral("delay"), 1},
		{QStringLiteral("installer/features"), static_cast<int>(UpdateInstaller::Feature::DetailedProgress)},
		{QStringLiteral("installer/delay"), 10},
		{QStringLiteral("installer/status"), QStringLiteral("test_status")},
	};
	QList<UpdateInfo> updates {
		{0, QStringLiteral("test_1"), {1,1,2}},
		{1, QStringLiteral("test_2"), {1,2,0}},
	};
	parametrize(config, updates);

	// trigger updates
	sptr updater {Updater::create(QStringLiteral("test"), config, this)};
	auto installer = getInstaller(updater.data());
	QVERIFY(installer);

	QSignalSpy globSpy{installer, &UpdateInstaller::updateGlobalProgress};
	QVERIFY(globSpy.isValid());
	QSignalSpy compSpy{installer, &UpdateInstaller::updateComponentProgress};
	QVERIFY(compSpy.isValid());
	QSignalSpy doneSpy{installer, &UpdateInstaller::installSucceeded};
	QVERIFY(doneSpy.isValid());

	auto model = installer->progressModel();
	QAbstractItemModelTester modelTester {
		model,
		QAbstractItemModelTester::FailureReportingMode::QtTest
	};

	installer->startInstall();
	QVERIFY(doneSpy.wait());
	QCOMPARE(doneSpy.size(), 1);
	QCOMPARE(globSpy.size(), 22);
	QCOMPARE(compSpy.size(), 20);

	// verify model data
	QCOMPARE(model->rowCount(), updates.size());
	for (auto i = 0; i < updates.size(); ++i) {
		QCOMPARE(model->data(model->index(i, 0), Qt::DisplayRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 0), Qt::ToolTipRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 1), Qt::DisplayRole).toString(), QStringLiteral("test_status"));
		QCOMPARE(model->data(model->index(i, 1), Qt::ToolTipRole).toString(), QStringLiteral("test_status"));
		QCOMPARE(model->data(model->index(i, 2), Qt::DisplayRole).toDouble(), 1.0);
		QCOMPARE(model->data(model->index(i, 2), Qt::ToolTipRole).toDouble(), 1.0);

		QCOMPARE(model->data(model->index(i, 0), ProgressModel::NameRole).toString(), updates[i].name());
		QCOMPARE(model->data(model->index(i, 0), ProgressModel::ProgressRole).toDouble(), 1.0);
		QCOMPARE(model->data(model->index(i, 0), ProgressModel::StatusRole).toString(), QStringLiteral("test_status"));
		QCOMPARE(model->data(model->index(i, 0), ProgressModel::UpdateInfoRole).value<UpdateInfo>(), updates[i]);
	}
}

void UpdateInstallerTest::parametrize(QVariantMap &params, const QList<UpdateInfo> &updates)
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

void UpdateInstallerTest::parametrize(QVariantMap &params, const QList<std::pair<QString, bool>> &eulas)
{
	params.insert(QStringLiteral("eulas/size"), eulas.size());
	for (auto i = 0; i < eulas.size(); ++i) {
		const auto &eula = eulas[i];
		params.insert(QStringLiteral("eulas/%1/text").arg(i), eula.first);
		params.insert(QStringLiteral("eulas/%1/required").arg(i), eula.second);
	}
}

UpdateInstaller *UpdateInstallerTest::getInstaller(Updater *updater)
{
	UpdateInstaller *installer = nullptr;
	[&](){
		QSignalSpy doneSpy{updater, &Updater::checkUpdatesDone};
		QVERIFY(doneSpy.isValid());
		updater->checkForUpdates();
		QVERIFY(doneSpy.wait());
		QCOMPARE(doneSpy.takeFirst()[0].value<Updater::State>(), Updater::State::NewUpdates);

		QSignalSpy installSpy{updater, &Updater::showInstaller};
		QVERIFY(installSpy.isValid());
		QVERIFY(updater->runUpdater());
		QCOMPARE(installSpy.size(), 1);
		installer = installSpy.takeFirst()[0].value<UpdateInstaller*>();
	}();
	return installer;
}

QTEST_MAIN(UpdateInstallerTest)

#include "tst_updateinstaller.moc"
