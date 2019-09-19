#include "qplaystoreupdaterbackend.h"
#include "qplaystoreupdateinstaller.h"
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniExceptionCleaner>
#include <QtAndroidExtras/private/qandroidactivityresultreceiver_p.h>
using namespace QtAutoUpdater;

Q_DECLARE_METATYPE(QAndroidJniObject)

const QString QPlayStoreUpdaterBackend::KeyDebug {QStringLiteral("debug")};
const QString QPlayStoreUpdaterBackend::KeyAutoResumeInstall {QStringLiteral("autoResumeInstall")};

QHash<QUuid, QPointer<QPlayStoreUpdaterBackend>> QPlayStoreUpdaterBackend::backends;

void QPlayStoreUpdaterBackend::registerNatives(JNIEnv *env)
{
	QAndroidJniExceptionCleaner _;

	// register natives:
	static const std::array<JNINativeMethod, 2> methods {
		JNINativeMethod{"reportCheckResult", "(Lcom/google/android/play/core/appupdate/AppUpdateInfo;)V", reinterpret_cast<void*>(&QPlayStoreUpdaterBackend::jniReportCheckResult)},
		JNINativeMethod{"onStateUpdate", "(Lcom/google/android/play/core/install/InstallState;)V", reinterpret_cast<void*>(&QPlayStoreUpdaterBackend::jniOnStateUpdate)}
	};

	const auto clazz = env->FindClass("de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper");
	Q_ASSERT_X(clazz, Q_FUNC_INFO, "Unable to find de.skycoder42.qtautoupdater.core.plugin.qplaystore.UpdateHelper - make shure the jar was added to your app.");
	const auto ok = env->RegisterNatives(clazz, methods.data(), methods.size());
	Q_ASSERT_X(ok >= 0, Q_FUNC_INFO, "Failed to register natives for de.skycoder42.qtautoupdater.core.plugin.qplaystore.UpdateHelper");
}

QPlayStoreUpdaterBackend::QPlayStoreUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{
#ifndef QT_NO_DEBUG
	assertEnums();
#endif

	qRegisterMetaType<QAndroidJniObject>();

	const auto receiverPrivate = QAndroidActivityResultReceiverPrivate::get(this);
	receiverPrivate->localToGlobalRequestCode.insert(InstallImmediateRequestCode, InstallImmediateRequestCode);
	receiverPrivate->globalToLocalRequestCode.insert(InstallImmediateRequestCode, InstallImmediateRequestCode);
	receiverPrivate->localToGlobalRequestCode.insert(InstallFlexibleRequestCode, InstallFlexibleRequestCode);
	receiverPrivate->globalToLocalRequestCode.insert(InstallFlexibleRequestCode, InstallFlexibleRequestCode);
}

QPlayStoreUpdaterBackend::~QPlayStoreUpdaterBackend()
{
	backends.remove(id);
}

QAndroidJniObject QPlayStoreUpdaterBackend::getAppUpdateInfo(const QString &infoId) const
{
	const auto obj = _updateInfoCache.object(infoId);
	return obj ? *obj : QAndroidJniObject{};
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
	QAndroidJniExceptionCleaner _;
	if (infos.size() != 1)
		return false;
	const auto &info = infos[0];

	const auto jInfo = _updateInfoCache.object(info.identifier().toString());
	if (!jInfo)
		return false;

	if (track) {
		return _updateHelper.callMethod<jboolean>("triggerUpdate",
												  "(ILandroid/app/Activity;Lcom/google/android/play/core/appupdate/AppUpdateInfo;)Z",
												  InstallImmediateRequestCode,
												  QtAndroid::androidActivity().object(),
												  jInfo->object());
	} else {
		_updateHelper.callMethod<void>("openInPlay",
									   "(Landroid/content/Context;Lcom/google/android/play/core/appupdate/AppUpdateInfo;)V",
									   QtAndroid::androidContext().object(),
									   jInfo->object());
		return true;
	}
}

QtAutoUpdater::UpdateInstaller *QPlayStoreUpdaterBackend::createInstaller()
{
	_installer = new QPlayStoreUpdateInstaller{_updateHelper, this};
	return _installer;
}

void QPlayStoreUpdaterBackend::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
	Q_UNUSED(data)
	if (receiverRequestCode == InstallImmediateRequestCode) {
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
	if (config()->value(KeyDebug, DefaultDebug).toBool()) {
		appUpdateManager = QAndroidJniObject {
			"com/google/android/play/core/appupdate/testing/FakeAppUpdateManager",
			"(Landroid/content/Context;)V",
			QtAndroid::androidContext().object()
		};
		setProperty("fakeAppUpdateManager", QVariant::fromValue(appUpdateManager));
	} else {
		appUpdateManager = QAndroidJniObject::callStaticObjectMethod("com/google/android/play/core/appupdate/AppUpdateManagerFactory",
																	 "create", "(Landroid/content/Context;)Lcom/google/android/play/core/appupdate/AppUpdateManager;",
																	 QtAndroid::androidContext().object());
	}
	if (!appUpdateManager.isValid())
		return false;

	_updateHelper = QAndroidJniObject {
		"de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper",
		"(Ljava/lang/String;Lcom/google/android/play/core/appupdate/AppUpdateManager;)V",
		QAndroidJniObject::fromString(id.toString(QUuid::WithoutBraces)).object(),
		appUpdateManager.object()
	};
	if (!_updateHelper.isValid())
		return false;
	backends.insert(id, this);

	if (config()->value(KeyAutoResumeInstall, QtAndroid::androidActivity().isValid()).toBool()) {
		_updateHelper.callMethod<void>("resumeStalledUpdate",
									   "(ILandroid/app/Activity;)V",
									   InstallIgnoredRequestCode,
									   QtAndroid::androidActivity().object());
	}

	return true;
}

#ifndef QT_NO_DEBUG
void QPlayStoreUpdaterBackend::assertEnums()
{
	QAndroidJniExceptionCleaner _;
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

	#define ASSERT_IS_ENUM(enumValue, field) ASSERT_ENUM((enumValue), "com/google/android/play/core/install/model/InstallStatus", (field))
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Unknown, "UNKNOWN");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Pending, "PENDING");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Downloading, "DOWNLOADING");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Installing, "INSTALLING");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Installed, "INSTALLED");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Failed, "FAILED");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Canceled, "CANCELED");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::RequiresUiIntent, "REQUIRES_UI_INTENT");
	ASSERT_IS_ENUM(QPlayStoreUpdateInstaller::InstallStatus::Downloaded, "DOWNLOADED");
	#undef ASSERT_IS_ENUM

	#define ASSERT_IEC_ENUM(enumValue, field) ASSERT_ENUM((enumValue), "com/google/android/play/core/install/model/InstallErrorCode", (field))
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::NoError, "NO_ERROR");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::NoErrorPartiallyAllowed, "NO_ERROR_PARTIALLY_ALLOWED");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorUnknown, "ERROR_UNKNOWN");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorApiNotAvailable, "ERROR_API_NOT_AVAILABLE");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorInvalidRequest, "ERROR_INVALID_REQUEST");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorInstallUnavailable, "ERROR_INSTALL_UNAVAILABLE");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorInstallNotAllowed, "ERROR_INSTALL_NOT_ALLOWED");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorDownloadNotPresent, "ERROR_DOWNLOAD_NOT_PRESENT");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorInstallInProgress, "ERROR_INSTALL_IN_PROGRESS");
	ASSERT_IEC_ENUM(QPlayStoreUpdateInstaller::InstallErrorCode::ErrorInternalError, "ERROR_INTERNAL_ERROR");
	#undef ASSERT_IEC_ENUM

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
	if (_installer)
		_installer->onStateUpdate(state);
}

void QPlayStoreUpdaterBackend::jniReportCheckResult(JNIEnv */*env*/, jobject updateHelper, jobject info)
{
	QAndroidJniExceptionCleaner _;
	QAndroidJniObject helper{updateHelper};
	const auto id = QUuid::fromString(helper.callObjectMethod<jstring>("id").toString());
	const auto backend = QPlayStoreUpdaterBackend::backends.value(id);
	if (backend) {
		QMetaObject::invokeMethod(backend, "reportCheckResult", Qt::QueuedConnection,
								  Q_ARG(QAndroidJniObject, QAndroidJniObject{info}));
	}
}

void QPlayStoreUpdaterBackend::jniOnStateUpdate(JNIEnv */*env*/, jobject updateHelper, jobject state)
{
	QAndroidJniExceptionCleaner _;
	QAndroidJniObject helper{updateHelper};
	const auto id = QUuid::fromString(helper.callObjectMethod<jstring>("id").toString());
	const auto backend = QPlayStoreUpdaterBackend::backends.value(id);
	if (backend) {
		QMetaObject::invokeMethod(backend, "onStateUpdate", Qt::QueuedConnection,
								  Q_ARG(QAndroidJniObject, QAndroidJniObject{state}));
	}
}

QList<UpdateInfo> QPlayStoreUpdaterBackend::parseInfo(const QAndroidJniObject &jInfo)
{
	QAndroidJniExceptionCleaner _;
	UpdateInfo info;
	info.setName(jInfo.callObjectMethod<jstring>("packageName").toString());
	info.setVersion({jInfo.callMethod<jint>("availableVersionCode")});
	info.setIdentifier(info.name());
	_updateInfoCache.insert(info.name(), new QAndroidJniObject{jInfo});
	return {info};
}
