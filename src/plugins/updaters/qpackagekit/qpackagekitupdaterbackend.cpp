#include "qpackagekitupdaterbackend.h"
#include "qpackagekitupdateinstaller.h"
#include <packagekitqt5/PackageKit/Daemon>
using namespace QtAutoUpdater;
using namespace PackageKit;

Q_LOGGING_CATEGORY(logPackageKitBackend, "qt.autoupdater.core.plugin.packagekit.backend")

const QString QPackageKitUpdaterBackend::KeyPackages {QStringLiteral("packages")};

QString QPackageKitUpdaterBackend::statusString(Transaction::Status status)
{
	switch (status) {
	case PackageKit::Transaction::StatusWait:
		return tr("Please wait…");
	case PackageKit::Transaction::StatusSetup:
		return tr("Setting up…");
	case PackageKit::Transaction::StatusRunning:
		return tr("Running…");
	case PackageKit::Transaction::StatusQuery:
		return tr("Querying database…");
	case PackageKit::Transaction::StatusInfo:
		return tr("Showing information…");
	case PackageKit::Transaction::StatusRemove:
		return tr("Removing packages…");
	case PackageKit::Transaction::StatusRefreshCache:
		return tr("Refreshing package cache…");
	case PackageKit::Transaction::StatusDownload:
		return tr("Downloading packages…");
	case PackageKit::Transaction::StatusInstall:
		return tr("Installing packages…");
	case PackageKit::Transaction::StatusUpdate:
		return tr("Updating packages…");
	case PackageKit::Transaction::StatusCleanup:
		return tr("Cleaning up…");
	case PackageKit::Transaction::StatusObsolete:
		return tr("Package obsolete!");
	case PackageKit::Transaction::StatusDepResolve:
		return tr("Resolving package dependencies…");
	case PackageKit::Transaction::StatusSigCheck:
		return tr("Verifying package signatures…");
	case PackageKit::Transaction::StatusTestCommit:
		return tr("Testing for commit…");
	case PackageKit::Transaction::StatusCommit:
		return tr("Committing updates…");
	case PackageKit::Transaction::StatusRequest:
		return tr("Sending request…");
	case PackageKit::Transaction::StatusFinished:
		return tr("Finished.");
	case PackageKit::Transaction::StatusCancel:
		return tr("Canceling…");
	case PackageKit::Transaction::StatusDownloadRepository:
		return tr("Downloading package repositories…");
	case PackageKit::Transaction::StatusDownloadPackagelist:
		return tr("Downloading package list…");
	case PackageKit::Transaction::StatusDownloadFilelist:
		return tr("Downloading file list…");
	case PackageKit::Transaction::StatusDownloadChangelog:
		return tr("Downloading changelog…");
	case PackageKit::Transaction::StatusDownloadGroup:
		return tr("Downloading package group…");
	case PackageKit::Transaction::StatusDownloadUpdateinfo:
		return tr("Downloading update infos…");
	case PackageKit::Transaction::StatusRepackaging:
		return tr("Repackaging…");
	case PackageKit::Transaction::StatusLoadingCache:
		return tr("Loading cache…");
	case PackageKit::Transaction::StatusScanApplications:
		return tr("Scanning applications…");
	case PackageKit::Transaction::StatusGeneratePackageList:
		return tr("Generating package list…");
	case PackageKit::Transaction::StatusWaitingForLock:
		return tr("Waiting for lock…");
	case PackageKit::Transaction::StatusWaitingForAuth:
		return tr("Waiting for authentication…");
	case PackageKit::Transaction::StatusScanProcessList:
		return tr("Scanning processes…");
	case PackageKit::Transaction::StatusCheckExecutableFiles:
		return tr("Verifying executables…");
	case PackageKit::Transaction::StatusCheckLibraries:
		return tr("Verifying libraries…");
	case PackageKit::Transaction::StatusCopyFiles:
		return tr("Copying files…");
	case PackageKit::Transaction::StatusRunHook:
		return tr("Running install hooks…");
	case PackageKit::Transaction::StatusUnknown:
		return {};
	default:
		Q_UNREACHABLE();
		return {};
	}
}

QPackageKitUpdaterBackend::QPackageKitUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

UpdaterBackend::Features QPackageKitUpdaterBackend::features() const
{
	return Feature::CheckProgress |
			Feature::PerformInstall;
}

void QPackageKitUpdaterBackend::checkForUpdates()
{
	if (_checkTrans)
		return;

	_updates.clear();
	_lastPercent = -1.0;
	_lastStatus.clear();

	_checkTrans = Daemon::getUpdates(Transaction::FilterInstalled);
	connect(_checkTrans, &Transaction::percentageChanged,
			this, &QPackageKitUpdaterBackend::percentageChanged);
	connect(_checkTrans, &Transaction::statusChanged,
			this, &QPackageKitUpdaterBackend::statusChanged);
	connect(_checkTrans, &Transaction::package,
			this, &QPackageKitUpdaterBackend::package);
	connect(_checkTrans, &Transaction::errorCode,
			this, &QPackageKitUpdaterBackend::errorCode);
	connect(_checkTrans, &Transaction::finished,
			this, &QPackageKitUpdaterBackend::finished);
}

void QPackageKitUpdaterBackend::abort(bool)
{
	if (_checkTrans->allowCancel())
		_checkTrans->cancel();
}

bool QPackageKitUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &, bool)
{
	return false;
}

UpdateInstaller *QPackageKitUpdaterBackend::createInstaller()
{
	return new QPackageKitUpdateInstaller{this};
}

bool QPackageKitUpdaterBackend::initialize()
{
	if (auto pData = config()->value(KeyPackages); pData)
		_packageFilter = readStringList(*pData, QLatin1Char(';'));
	if (_packageFilter.isEmpty()) {
		qCCritical(logPackageKitBackend) << "Configuration for packagekit must contain 'packages'";
		return false;
	} else
		return true;
}

void QPackageKitUpdaterBackend::percentageChanged()
{
	const auto perc = _checkTrans->percentage();
	_lastPercent = perc > 100 ? -1.0 : perc / 100.0;
	emit checkProgress(_lastPercent, _lastStatus);
}

void QPackageKitUpdaterBackend::statusChanged()
{
	_lastStatus = statusString(_checkTrans->status());
	emit checkProgress(_lastPercent, _lastStatus);
}

void QPackageKitUpdaterBackend::package(Transaction::Info info, const QString &packageID)
{
	switch (info) {
	case Transaction::InfoLow:
	case Transaction::InfoEnhancement:
	case Transaction::InfoNormal:
	case Transaction::InfoBugfix:
	case Transaction::InfoImportant:
	case Transaction::InfoSecurity:{
		// TODO check if secondary info is given
		qCDebug(logPackageKitBackend).nospace() << packageID;
		auto parts = packageID.split(QStringLiteral(";"));
		if (!_packageFilter.contains(parts[0]))
			break;
		UpdateInfo updInfo;
		updInfo.setName(parts[0]);
		updInfo.setVersion(QVersionNumber::fromString(parts[1]));
		updInfo.setIdentifier(packageID);
		_updates.append(updInfo);
		break;
	}
	default:
		break;
	}
}

void QPackageKitUpdaterBackend::errorCode(Transaction::Error code, const QString &details)
{
	_checkTrans->disconnect(this);
	_checkTrans->deleteLater();
	qCCritical(logPackageKitBackend) << "Update-Check-Transaction failed with code" << code
									 << "and message:" << qUtf8Printable(details);
	emit checkDone(false);
	_updates.clear();
}

void QPackageKitUpdaterBackend::finished(Transaction::Exit status)
{
	_checkTrans->disconnect(this);
	_checkTrans->deleteLater();
	if (status == Transaction::ExitSuccess)
		emit checkDone(true, _updates);
	else
		emit checkDone(false);
	_updates.clear();
}
