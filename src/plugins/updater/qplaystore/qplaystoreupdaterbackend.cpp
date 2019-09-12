#include "qplaystoreupdaterbackend.h"
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniExceptionCleaner>

QPlayStoreUpdaterBackend::QPlayStoreUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

QtAutoUpdater::UpdaterBackend::Features QPlayStoreUpdaterBackend::features() const
{
	return Feature::CheckUpdates;
}

void QPlayStoreUpdaterBackend::checkForUpdates()
{

}

void QPlayStoreUpdaterBackend::abort(bool force)
{

}

bool QPlayStoreUpdaterBackend::triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track)
{
	return false;
}

QtAutoUpdater::UpdateInstaller *QPlayStoreUpdaterBackend::createInstaller()
{
	return nullptr;
}

bool QPlayStoreUpdaterBackend::initialize()
{
	QAndroidJniExceptionCleaner _;

	_appUpdateManager = QAndroidJniObject::callStaticObjectMethod("com/google/android/play/core/appupdate/AppUpdateManagerFactory",
																  "create", "Lcom/google/android/play/core/appupdate/AppUpdateManager;(Landroid/content/Context;)",
																  QtAndroid::androidContext().object());
	if (!_appUpdateManager.isValid())
		return false;

	return true;
}
