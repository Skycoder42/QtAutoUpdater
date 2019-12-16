#ifndef QPACKAGEKITUPDATEINSTALLER_H
#define QPACKAGEKITUPDATEINSTALLER_H

#include <packagekitqt5/PackageKit/Transaction>

#include <QtCore/QPointer>
#include <QtCore/QLoggingCategory>
#include <QtAutoUpdaterCore/UpdateInstaller>

class QPackageKitUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	explicit QPackageKitUpdateInstaller(QObject *parent = nullptr);

	Features features() const override;

public Q_SLOTS:
	void cancelInstall() override;
	void eulaHandled(const QVariant &id, bool accepted) override;
	void restartApplication() override;

protected:
	void startInstallImpl() override;

private Q_SLOTS:
	void percentageChanged();
	void statusChanged();
	void itemProgress(const QString &itemID, PackageKit::Transaction::Status status, uint percentage);
	void eulaRequired(const QString &eulaID, const QString &packageID, const QString &vendor, const QString &licenseAgreement);
	void errorCode(PackageKit::Transaction::Error code, const QString &details);
	void finished(PackageKit::Transaction::Exit status);
	void requireRestart(PackageKit::Transaction::Restart type, const QString &packageID);

private:
	QPointer<PackageKit::Transaction> _installTrans;

	double _lastPercent = -1.0;
	QString _lastStatus;
	bool _needsRestart = false;

	QHash<QString, QString> _packageMap;
};

Q_DECLARE_LOGGING_CATEGORY(logPackageKitInstaller)

#endif // QPACKAGEKITUPDATEINSTALLER_H
