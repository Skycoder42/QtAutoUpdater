#include "qpackagekitupdaterbackend.h"
#include <Daemon>
using namespace QtAutoUpdater;
using namespace PackageKit;

QPackageKitUpdaterBackend::QPackageKitUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent}
{}

UpdaterBackend::Features QPackageKitUpdaterBackend::features() const
{
	return Feature::CheckProgress |
			Feature::PerformInstall |
			Feature::InstallSelected;
}

void QPackageKitUpdaterBackend::checkForUpdates()
{
	if (_checkTrans)
		return;

	_updates.clear();
	_checkTrans = Daemon::getUpdates(Transaction::FilterInstalled);
	connect(_checkTrans, &Transaction::percentageChanged,
			this, &QPackageKitUpdaterBackend::percentageChanged);
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

UpdateInstaller *QPackageKitUpdaterBackend::installUpdates(const QList<UpdateInfo> &infos)
{
	Q_UNIMPLEMENTED();
	return nullptr;
}

bool QPackageKitUpdaterBackend::initialize()
{
	auto pData = config()->value(QStringLiteral("packages"));
	if (pData) {
		_packageFilter = pData->toString().split(QLatin1Char(';'));
		return true;
	} else {
		qCCritical(logCat()) << "Configuration for packagekit must contain 'packages'";
		return false;
	}
}

void QPackageKitUpdaterBackend::percentageChanged()
{
	const auto perc = _checkTrans->percentage();
	emit checkProgress(perc > 100 ? 1.0 : perc / 100.0, tr("Checking for updates…"));  // TODO use status signal
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
	qCCritical(logCat()) << "Update-Check-Transaction failed with code" << code
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
