#include "qplaystoreupdateinstaller.h"
#include "qplaystoreupdaterbackend.h"
#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniExceptionCleaner>
using namespace QtAutoUpdater;

QPlayStoreUpdateInstaller::QPlayStoreUpdateInstaller(const QAndroidJniObject &updateHelper, QPlayStoreUpdaterBackend *parent) :
	UpdateInstaller{parent},
	_backend{parent},
	_updateHelper{updateHelper}
{}

UpdateInstaller::Features QPlayStoreUpdateInstaller::features() const
{
	return Feature::CanCancel;
}

void QPlayStoreUpdateInstaller::cancelInstall()
{
	_canceled = true;
	_done = true;
	emit installFailed(tr("The installation was canceled by the user!"));
}

void QPlayStoreUpdateInstaller::eulaHandled(const QVariant &id, bool accepted)
{
	Q_UNUSED(id)
	Q_UNUSED(accepted)
}

void QPlayStoreUpdateInstaller::restartApplication()
{
	_updateHelper.callMethod<void>("triggerRestart");
}

void QPlayStoreUpdateInstaller::startInstallImpl()
{
	QAndroidJniExceptionCleaner _;
	auto updates = components();
	if (updates.size() != 1) {
		_done = true;
		emit installFailed(tr("Can only install one update at a time"));
		return;
	}

	auto jInfo = _backend->getAppUpdateInfo(updates[0].identifier().toString());
	if (!jInfo.isValid()) {
		_done = true;
		emit installFailed(tr("No update information available! Try to check for updates again to fix this problem."));
		return;
	}

	const auto ok = _updateHelper.callMethod<jboolean>("startUpdate",
													   "(ILandroid/app/Activity;Lcom/google/android/play/core/appupdate/AppUpdateInfo;)Z",
													   QPlayStoreUpdaterBackend::InstallFlexibleRequestCode,
													   QtAndroid::androidActivity().object(),
													   jInfo.object());
	if (!ok) {
		_done = true;
		emit installFailed(tr("Failed to initiate an update!"));
	}
}

void QPlayStoreUpdateInstaller::onStateUpdate(const QAndroidJniObject &state)
{
	QAndroidJniExceptionCleaner _;

	if (_canceled)
		return;

	if (state.isValid()) {
		switch (state.callMethod<jint>("installStatus")) {
		case InstallStatus::Unknown:
			emit updateGlobalProgress(-1.0, tr("Unknown state…"));
			break;
		case InstallStatus::Pending:
			emit updateGlobalProgress(-1.0, tr("Update installation is pending. Please wait…"));
			break;
		case InstallStatus::Downloading:
			emit updateGlobalProgress(-1.0, tr("Downloading update…"));
			break;
		case InstallStatus::Downloaded:
			emit updateGlobalProgress(-1.0, tr("Update downloaded"));
			_updateHelper.callMethod<void>("completeUpdate");
			break;
		case InstallStatus::Installing:
			emit updateGlobalProgress(-1.0, tr("Installing update…"));
			break;
		case InstallStatus::Installed:
			emit updateGlobalProgress(-1.0, tr("Update installed"));
			_done = true;
			emit installSucceeded(true);
			break;
		case InstallStatus::Failed:
			emit updateGlobalProgress(-1.0, tr("Update failed!"));
			_done = true;
			emit installFailed(errorMessage(static_cast<InstallErrorCode>(state.callMethod<jint>("installErrorCode"))));
			break;
		case InstallStatus::Canceled:
			emit updateGlobalProgress(-1.0, tr("Update was canceled!"));
			_done = true;
			emit installFailed(tr("The installation was canceled by the user!"));
			break;
		case InstallStatus::RequiresUiIntent:
			emit updateGlobalProgress(-1.0, tr("Waiting for user confirmation…"));
			break;
		default:
			Q_UNREACHABLE();
			break;
		}
	}
}

QString QPlayStoreUpdateInstaller::errorMessage(InstallErrorCode code)
{
	switch (code) {
	case QPlayStoreUpdateInstaller::NoError:
		return tr("No error occured.");
	case QPlayStoreUpdateInstaller::NoErrorPartiallyAllowed:
		return tr("No error occured, but installing is only partially allowed.");
	case QPlayStoreUpdateInstaller::ErrorUnknown:
		return tr("Unknown error.");
	case QPlayStoreUpdateInstaller::ErrorApiNotAvailable:
		return tr("Update-Installation API is not available on this device!");
	case QPlayStoreUpdateInstaller::ErrorInvalidRequest:
		return tr("An invalid request was made!");
	case QPlayStoreUpdateInstaller::ErrorInstallUnavailable:
		return tr("Update-Installation is not available on this device!");
	case QPlayStoreUpdateInstaller::ErrorInstallNotAllowed:
		return tr("Update-Installation is not allowed");
	case QPlayStoreUpdateInstaller::ErrorDownloadNotPresent:
		return tr("No downloaded update was found!");
	case QPlayStoreUpdateInstaller::ErrorInstallInProgress:
		return tr("Another installation is already in progress!");
	case QPlayStoreUpdateInstaller::ErrorInternalError:
		return tr("An internal error occured!");
	default:
		Q_UNREACHABLE();
		return {};
	}
}
