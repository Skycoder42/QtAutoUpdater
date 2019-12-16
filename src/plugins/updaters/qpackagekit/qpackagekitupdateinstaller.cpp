#include "qpackagekitupdateinstaller.h"
#include "qpackagekitupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <packagekitqt5/PackageKit/Daemon>
#include <unistd.h>
using namespace QtAutoUpdater;
using namespace PackageKit;

Q_LOGGING_CATEGORY(logPackageKitInstaller, "qt.autoupdater.core.plugin.packagekit.installer")

QPackageKitUpdateInstaller::QPackageKitUpdateInstaller(QObject *parent) :
	UpdateInstaller{parent}
{}

QtAutoUpdater::UpdateInstaller::Features QPackageKitUpdateInstaller::features() const
{
	return Feature::SelectComponents |
			Feature::DetailedProgress |
			Feature::CanCancel;
}

void QPackageKitUpdateInstaller::cancelInstall()
{
	if (_installTrans && _installTrans->allowCancel())
		_installTrans->cancel();
	else
		qCWarning(logPackageKitInstaller) << "Canceling is not allowed!";
}

void QPackageKitUpdateInstaller::eulaHandled(const QVariant &id, bool accepted)
{
	if (accepted) {
		auto eulaTrans = Daemon::acceptEula(id.toString());
		connect(eulaTrans, &Transaction::finished,
				this, [this, eulaTrans](Transaction::Exit status) {
			eulaTrans->deleteLater();
			switch (status) {
			case PackageKit::Transaction::ExitSuccess:
				startInstallImpl();
				break;
			case PackageKit::Transaction::ExitEulaRequired:
				emit installFailed(tr("EULA was rejected!"));
				break;
			default:
				finished(status);
				break;
			}
		});
	} else
		emit installFailed(tr("EULA was rejected!"));
}

void QPackageKitUpdateInstaller::restartApplication()
{
	QByteArrayList argList;
	argList.reserve(QCoreApplication::arguments().size());
	for (const auto &arg : QCoreApplication::arguments())
		argList.append(arg.toUtf8());
	QVector<char *> argv;
	argv.resize(argList.size());
	for (auto i = 0; i < argv.size(); ++i)
		argv[i] = argList[i].data();
	::execv(qUtf8Printable(QCoreApplication::applicationFilePath()), argv.data());
	qCCritical(logPackageKitInstaller) << "Failed to restart application";
}

void QPackageKitUpdateInstaller::startInstallImpl()
{
	if (_installTrans)
		return;

	_lastPercent = -1.0;
	_lastStatus.clear();
	_needsRestart = false;
	_packageMap.clear();

	_packageMap.reserve(components().size());
	for (const auto &info : components()) {
		const auto id = info.identifier().toString();
		const auto name = id.split(QLatin1Char(';')).first();
		_packageMap.insert(name, id);
	}

	_installTrans = Daemon::updatePackages(_packageMap.values(),
										   Transaction::TransactionFlagOnlyTrusted);
	connect(_installTrans, &Transaction::percentageChanged,
			this, &QPackageKitUpdateInstaller::percentageChanged);
	connect(_installTrans, &Transaction::statusChanged,
			this, &QPackageKitUpdateInstaller::statusChanged);
	connect(_installTrans, &Transaction::itemProgress,
			this, &QPackageKitUpdateInstaller::itemProgress);
	connect(_installTrans, &Transaction::eulaRequired,
			this, &QPackageKitUpdateInstaller::eulaRequired);
	connect(_installTrans, &Transaction::errorCode,
			this, &QPackageKitUpdateInstaller::errorCode);
	connect(_installTrans, &Transaction::finished,
			this, &QPackageKitUpdateInstaller::finished);
	connect(_installTrans, &Transaction::requireRestart,
			this, &QPackageKitUpdateInstaller::requireRestart);
}

void QPackageKitUpdateInstaller::percentageChanged()
{
	const auto perc = _installTrans->percentage();
	_lastPercent = perc > 100 ? -1.0 : perc / 100.0;
	emit updateGlobalProgress(_lastPercent, _lastStatus);
}

void QPackageKitUpdateInstaller::statusChanged()
{
	_lastStatus = QPackageKitUpdaterBackend::statusString(_installTrans->status());
	emit updateGlobalProgress(_lastPercent, _lastStatus);
}

void QPackageKitUpdateInstaller::itemProgress(const QString &itemID, Transaction::Status status, uint percentage)
{
	emit updateComponentProgress(_packageMap.value(itemID),
								 percentage > 100 ? -1.0 : percentage / 100.0,
								 QPackageKitUpdaterBackend::statusString(status));
}

void QPackageKitUpdateInstaller::eulaRequired(const QString &eulaID, const QString &packageID, const QString &vendor, const QString &licenseAgreement)
{
	emit updateGlobalProgress(-1.0, tr("Waiting for EULA to be accepted…"));
	emit showEula(eulaID,
				  tr("<h1>Eula of package %1</h1>"
					 "<p>%2</p>"
					 "<p>Package Vendor: %3</p>")
				  .arg(packageID, licenseAgreement, vendor),
				  true);
}

void QPackageKitUpdateInstaller::errorCode(Transaction::Error code, const QString &details)
{
	_installTrans->disconnect(this);
	_installTrans->deleteLater();
	qCCritical(logPackageKitInstaller) << "Update-Install-Transaction failed with code" << code
									   << "and message:" << qUtf8Printable(details);
	emit installFailed(details);
}

void QPackageKitUpdateInstaller::finished(Transaction::Exit status)
{
	if (_installTrans) {
		_installTrans->disconnect(this);
		_installTrans->deleteLater();
	}

	switch (status) {
	case Transaction::ExitSuccess:
		emit installSucceeded(_needsRestart);
		break;
	case Transaction::ExitFailed:
	case Transaction::ExitUnknown:
		emit installFailed(tr("Installation failed!"));
		break;
	case Transaction::ExitCancelled:
	case Transaction::ExitCancelledPriority:
	case Transaction::ExitKilled:
		emit installFailed(tr("Installation was canceled."));
		break;
	case Transaction::ExitKeyRequired:
		emit installFailed(tr("A key was required!"));
		break;
	case Transaction::ExitMediaChangeRequired:
		emit installFailed(tr("Media exchange was required!"));
		break;
	case Transaction::ExitNeedUntrusted:
		emit installFailed(tr("Cannot install untrusted packages!"));
		break;
	case Transaction::ExitRepairRequired:
		emit installFailed(tr("The system is corrupted and needs to be repaired!"));
		break;
	case Transaction::ExitEulaRequired:
		// ignored, already handled by eulaRequired
		break;
	}
}

void QPackageKitUpdateInstaller::requireRestart(Transaction::Restart type, const QString &packageID)
{
	Q_UNUSED(packageID)
	_needsRestart = type == Transaction::RestartApplication;
}
