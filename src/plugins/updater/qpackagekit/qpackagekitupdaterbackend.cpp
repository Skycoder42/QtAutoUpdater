#include "qpackagekitupdaterbackend.h"
#include <Daemon>
#include <QDebug>
using namespace QtAutoUpdater;
using namespace PackageKit;

QPackageKitUpdaterBackend::QPackageKitUpdaterBackend(QObject *parent) :
	UpdaterBackend{parent}
{}

UpdaterBackend::Features QPackageKitUpdaterBackend::features() const
{
	return Feature::CheckProgress |
			Feature::PerformInstall |
			Feature::InstallSelected;
}

bool QPackageKitUpdaterBackend::initialize(const QVariantMap &arguments, AdminAuthoriser *authoriser)
{
	return true;
}

UpdateInstaller *QPackageKitUpdaterBackend::installUpdates(const QList<UpdateInfo> &infos)
{

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

bool QPackageKitUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos)
{

}

void QPackageKitUpdaterBackend::percentageChanged()
{
	const auto perc = _checkTrans->percentage();
	if (perc > 100)
		emit updateProgress(-1.0, tr("Checking for updates…"));  // TODO use status signal
	else
		emit updateProgress(perc / 100.0, tr("Checking for updates…"));
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

void QPackageKitUpdaterBackend::errorCode(Transaction::Error, const QString &details)
{
	_checkTrans->disconnect(this);
	_checkTrans->deleteLater();
	emit error(details);
	_updates.clear();
}

void QPackageKitUpdaterBackend::finished(Transaction::Exit status)
{
	_checkTrans->disconnect(this);
	_checkTrans->deleteLater();
	if (status == Transaction::ExitSuccess)
		emit checkDone(_updates);
	else
		emit error(tr("Unknown Error"));
	_updates.clear();
}
