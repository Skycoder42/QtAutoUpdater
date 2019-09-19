#ifndef QPLAYSTOREUPDATERBACKEND_H
#define QPLAYSTOREUPDATERBACKEND_H

#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtCore/QCache>
#include <QtCore/QUuid>

#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidActivityResultReceiver>

class QPlayStoreUpdateInstaller;
class QPlayStoreUpdaterBackend : public QtAutoUpdater::UpdaterBackend, public QAndroidActivityResultReceiver
{
	Q_OBJECT

public:
	static const QString KeyDebug;
	static const QString KeyAutoResumeInstall;

	static constexpr bool DefaultDebug {false};

	enum UpdateAvailability : int {
		Unknown = 0x00000000,
		UpdateNotAvailable = 0x00000001,
		UpdateAvailable = 0x00000002,
		DeveloperTriggeredUpdateInProgress = 0x00000003
	};
	Q_ENUM(UpdateAvailability)

	enum UpdateResult : int {
		ResultOk = static_cast<int>(0xffffffff),
		ResultCanceled = 0x00000000,
		ResultInAppUpdateFailed = 0x00000001
	};
	Q_ENUM(UpdateResult)

	static constexpr jint InstallImmediateRequestCode = 0x09714831; // random hard coded value
	static constexpr jint InstallFlexibleRequestCode = 0x09714832; // random hard coded value
	static constexpr jint InstallIgnoredRequestCode = 0x09714832; // random hard coded value

	static void registerNatives(JNIEnv *env);

	explicit QPlayStoreUpdaterBackend(QString &&key, QObject *parent = nullptr);
	~QPlayStoreUpdaterBackend() override;

	QAndroidJniObject getAppUpdateInfo(const QString &infoId) const;

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

	// QAndroidActivityResultReceiver interface
	void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void reportCheckResult(const QAndroidJniObject &info);
	void onStateUpdate(const QAndroidJniObject &state);

private:
#ifndef QT_NO_DEBUG
	static void assertEnums();
#endif

	static QHash<QUuid, QPointer<QPlayStoreUpdaterBackend>> backends;

	QUuid id = QUuid::createUuid();
	QAndroidJniObject _updateHelper;
	QCache<QString, QAndroidJniObject> _updateInfoCache {10};

	QPointer<QPlayStoreUpdateInstaller> _installer;

	static void jniReportCheckResult(JNIEnv *env, jobject updateHelper, jobject info);
	static void jniOnStateUpdate(JNIEnv *env, jobject updateHelper, jobject state);

	QList<QtAutoUpdater::UpdateInfo> parseInfo(const QAndroidJniObject &jInfo);
};

#endif // QPLAYSTOREUPDATERBACKEND_H
