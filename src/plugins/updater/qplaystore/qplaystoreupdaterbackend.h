#ifndef QPLAYSTOREUPDATERBACKEND_H
#define QPLAYSTOREUPDATERBACKEND_H

#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtCore/QCache>

#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidActivityResultReceiver>

extern "C" {
	JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_reportCheckResult(JNIEnv *env, jobject updateHelper, jobject info);
	JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_onStateUpdate(JNIEnv *env, jobject updateHelper, jobject state);
}

class QPlayStoreUpdaterBackend : public QtAutoUpdater::UpdaterBackend, public QAndroidActivityResultReceiver
{
	Q_OBJECT

public:
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

	explicit QPlayStoreUpdaterBackend(QString &&key, QObject *parent = nullptr);
	~QPlayStoreUpdaterBackend() override;

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

	// QAndroidActivityResultReceiver interface
	void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;

protected:
	bool initialize() override;

private:
	static constexpr jint InstallUpdatesRequestCode = 0x09714831; // random hard coded value

#ifndef QT_NO_DEBUG
	static void assertEnums();
#endif

	friend JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_reportCheckResult(JNIEnv *env, jobject updateHelper, jobject info);
	friend JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_onStateUpdate(JNIEnv *env, jobject updateHelper, jobject state);

	static QHash<jobject, QPointer<QPlayStoreUpdaterBackend>> backends;

	QAndroidJniObject _updateHelper;
	QCache<QString, QAndroidJniObject> _updateInfoCache {100};

	void reportCheckResult(const QAndroidJniObject &info);
	void onStateUpdate(const QAndroidJniObject &state);

	QList<QtAutoUpdater::UpdateInfo> parseInfo(const QAndroidJniObject &jInfo);
};

#endif // QPLAYSTOREUPDATERBACKEND_H
