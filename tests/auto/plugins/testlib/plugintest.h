#ifndef PLUGINTEST_H
#define PLUGINTEST_H

#include <chrono>

#include <QtCore>
#include <QtAutoUpdaterCore>
#include <QtTest>

#ifndef TEST_FRIEND
#define TEST_FRIEND QObject
#endif

class PluginTest : public QObject
{
	Q_OBJECT

protected:
	using sptr = QScopedPointer<QtAutoUpdater::UpdaterBackend, QScopedPointerDeleteLater>;

	virtual bool init();
	virtual bool cleanup();

	virtual QString backend() const = 0;
	virtual QVariantMap config() = 0;
	virtual QVariantMap performConfig();

	virtual QList<QtAutoUpdater::UpdateInfo> createInfos(const QVersionNumber &versionFrom, const QVersionNumber &versionTo) = 0;
	virtual bool simulateInstall(const QVersionNumber &version) = 0;
	virtual bool prepareUpdate(const QVersionNumber &version) = 0;

	virtual bool canAbort(bool hard) const = 0;
	virtual bool cancelState() const = 0;

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testPluginLoadable();
	void testUpdateCheck_data();
	void testUpdateCheck();
	void testTriggerInstall();
	void testPerformInstall();

private:
	friend class TEST_FRIEND;
	QtAutoUpdater::UpdaterBackend *loadBackend(bool asPerform = false);
};

#define TEST_WRAP_BEGIN \
	auto ok = false; \
	[&]() {

#define TEST_WRAP_END \
		ok = true; \
	}(); \
	return ok;

#endif // PLUGINTEST_H
