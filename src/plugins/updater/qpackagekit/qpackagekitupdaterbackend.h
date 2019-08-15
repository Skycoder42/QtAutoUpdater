#ifndef QPACKAGEKITUPDATERBACKEND_H
#define QPACKAGEKITUPDATERBACKEND_H

#include <transaction.h>

#include <QtAutoUpdaterCore/UpdaterBackend>

class QPackageKitUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	explicit QPackageKitUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;
	QtAutoUpdater::UpdateInstaller *installUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void percentageChanged();
	void package(PackageKit::Transaction::Info info, const QString &packageID);
	void errorCode(PackageKit::Transaction::Error code, const QString &details);
	void finished(PackageKit::Transaction::Exit status);

private:
	QStringList _packageFilter;
	PackageKit::Transaction *_checkTrans = nullptr;
	QList<QtAutoUpdater::UpdateInfo> _updates;
};

#endif // QPACKAGEKITUPDATERBACKEND_H
