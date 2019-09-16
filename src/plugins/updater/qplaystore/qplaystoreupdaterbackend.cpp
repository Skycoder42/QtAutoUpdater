#include "qplaystoreupdaterbackend.h"
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniExceptionCleaner>
#include <QtAndroidExtras/private/qandroidactivityresultreceiver_p.h>
using namespace QtAutoUpdater;

QHash<jobject, QPointer<QPlayStoreUpdaterBackend>> QPlayStoreUpdaterBackend::backends;

QPlayStoreUpdaterBackend::QPlayStoreUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{
#ifndef QT_NO_DEBUG
	assertEnums();
#endif
	const auto receiverPrivate = QAndroidActivityResultReceiverPrivate::get(this);
	receiverPrivate->localToGlobalRequestCode.insert(InstallUpdatesRequestCode, InstallUpdatesRequestCode);
	receiverPrivate->globalToLocalRequestCode.insert(InstallUpdatesRequestCode, InstallUpdatesRequestCode);
}

QPlayStoreUpdaterBackend::~QPlayStoreUpdaterBackend()
{
	backends.remove(_updateHelper.object());
}

UpdaterBackend::Features QPlayStoreUpdaterBackend::features() const
{
	return Feature::CheckUpdates |
			Feature::ParallelTrigger |
			Feature::PerformInstall;
}

void QPlayStoreUpdaterBackend::checkForUpdates()
{
	_updateHelper.callMethod<void>("startUpdateCheck");
}

void QPlayStoreUpdaterBackend::abort(bool force)
{
	Q_UNUSED(force)
}

bool QPlayStoreUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
	auto ok = true;
	for (const auto &info : infos) {
		auto jInfo = _updateInfoCache.take(info.identifier().toString());
		if (!jInfo)
			continue;
		// workaround -> add one to one mapping of codes to the receiver
		ok = _updateHelper.callMethod<jboolean>("triggerUpdate",
												"(ILandroid/app/Activity;Lcom/google/android/play/core/appupdate/AppUpdateInfo;)Z",
												track ? InstallUpdatesRequestCode : InstallUpdatesRequestCode + 1,
												QtAndroid::androidActivity().object(),
												nullptr) && ok;
		delete jInfo;
	}
	return ok;
}

QtAutoUpdater::UpdateInstaller *QPlayStoreUpdaterBackend::createInstaller()
{
	Q_UNIMPLEMENTED();
	return nullptr;
}

void QPlayStoreUpdaterBackend::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
	if (receiverRequestCode == InstallUpdatesRequestCode) {
		switch (resultCode) {
		case UpdateResult::ResultOk:
			emit triggerInstallDone(true);
			break;
		case UpdateResult::ResultCanceled:
		case UpdateResult::ResultInAppUpdateFailed:
			emit triggerInstallDone(false);
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}
}

bool QPlayStoreUpdaterBackend::initialize()
{
	QAndroidJniExceptionCleaner _;

	QAndroidJniObject appUpdateManager;
	if (config()->value(QStringLiteral("debug"), false).toBool()) {
		appUpdateManager = QAndroidJniObject {
			"com/google/android/play/core/appupdate/testing/FakeAppUpdateManager",
			"(Landroid/content/Context;)V",
			QtAndroid::androidContext().object()
		};
	} else {
		appUpdateManager = QAndroidJniObject::callStaticObjectMethod("com/google/android/play/core/appupdate/AppUpdateManagerFactory",
																	 "create", "(Landroid/content/Context;)Lcom/google/android/play/core/appupdate/AppUpdateManager;",
																	 QtAndroid::androidContext().object());
	}
	if (!appUpdateManager.isValid())
		return false;

	_updateHelper = QAndroidJniObject {
		"de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper",
		"(Lcom/google/android/play/core/appupdate/AppUpdateManager;)V",
		appUpdateManager.object()
	};
	if (!_updateHelper.isValid())
		return false;
	backends.insert(_updateHelper.object(), this);

	if (config()->value(QStringLiteral("autoResumeInstall"), QtAndroid::androidActivity().isValid()).toBool()) {
		_updateHelper.callMethod<void>("resumeStalledUpdate",
									   "(ILandroid/app/Activity;)V",
									   InstallUpdatesRequestCode + 1,
									   QtAndroid::androidActivity().object());
	}

	return true;
}

#ifndef QT_NO_DEBUG
void QPlayStoreUpdaterBackend::assertEnums()
{
	#define ASSERT_ENUM(enumValue, jClass, field) Q_ASSERT(static_cast<jint>(enumValue) == QAndroidJniObject::getStaticField<jint>(jClass, field))

	#define ASSERT_UA_ENUM(enumValue, field) ASSERT_ENUM((enumValue), "com/google/android/play/core/install/model/UpdateAvailability", (field))
	ASSERT_UA_ENUM(UpdateAvailability::Unknown, "UNKNOWN");
	ASSERT_UA_ENUM(UpdateAvailability::UpdateNotAvailable, "UPDATE_NOT_AVAILABLE");
	ASSERT_UA_ENUM(UpdateAvailability::UpdateAvailable, "UPDATE_AVAILABLE");
	ASSERT_UA_ENUM(UpdateAvailability::DeveloperTriggeredUpdateInProgress, "DEVELOPER_TRIGGERED_UPDATE_IN_PROGRESS");
	#undef ASSERT_UA_ENUM

	ASSERT_ENUM(UpdateResult::ResultOk, "android/app/Activity", "RESULT_OK");
	ASSERT_ENUM(UpdateResult::ResultOk, "android/app/Activity", "RESULT_CANCELED");
	ASSERT_ENUM(UpdateResult::ResultOk, "com/google/android/play/core/install/model/ActivityResult", "RESULT_IN_APP_UPDATE_FAILED");

	#undef ASSERT_ENUM
}
#endif

void QPlayStoreUpdaterBackend::reportCheckResult(const QAndroidJniObject &info)
{
	QAndroidJniExceptionCleaner _;

	if (info.isValid()) {
		switch (info.callMethod<jint>("updateAvailability")) {
		case UpdateAvailability::Unknown:
			emit checkDone(false);
			break;
		case UpdateAvailability::UpdateNotAvailable:
			emit checkDone(true);
			break;
		case UpdateAvailability::UpdateAvailable:
			emit checkDone(true, parseInfo(info));
			break;
		case UpdateAvailability::DeveloperTriggeredUpdateInProgress:
			triggerUpdates(parseInfo(info), true);
			break;
		}
	} else
		emit checkDone(false);
}

void QPlayStoreUpdaterBackend::onStateUpdate(const QAndroidJniObject &state)
{
	Q_UNIMPLEMENTED();
}

QList<UpdateInfo> QPlayStoreUpdaterBackend::parseInfo(const QAndroidJniObject &jInfo)
{
	UpdateInfo info;
	info.setName(jInfo.callObjectMethod("packageName", "()Ljava/lang/String;").toString());
	info.setVersion({jInfo.callMethod<jint>("availableVersionCode")});
	info.setIdentifier(info.name());
	_updateInfoCache.insert(info.name(), new QAndroidJniObject{jInfo});
	return {info};
}

// ------------- JNI implementations -------------

extern "C" {

JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_reportCheckResult(JNIEnv */*env*/, jobject updateHelper, jobject info)
{
	auto backend = QPlayStoreUpdaterBackend::backends.value(updateHelper);
	if (backend)
		backend->reportCheckResult(QAndroidJniObject{info});
}

JNIEXPORT void JNICALL Java_de_skycoder42_qtautoupdater_core_plugin_qplaystore_UpdateHelper_onStateUpdate(JNIEnv */*env*/, jobject updateHelper, jobject state)
{
	auto backend = QPlayStoreUpdaterBackend::backends.value(updateHelper);
	if (backend)
		backend->onStateUpdate(QAndroidJniObject{state});
}

}
