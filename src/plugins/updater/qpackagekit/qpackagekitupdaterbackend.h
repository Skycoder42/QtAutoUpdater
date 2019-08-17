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
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void percentageChanged();
	void statusChanged();
	void package(PackageKit::Transaction::Info info, const QString &packageID);
	void errorCode(PackageKit::Transaction::Error code, const QString &details);
	void finished(PackageKit::Transaction::Exit status);

private:
	QStringList _packageFilter;
	PackageKit::Transaction *_checkTrans = nullptr;
	QList<QtAutoUpdater::UpdateInfo> _updates;

	double _lastPercent = -1.0;
	QString _lastStatus;
};

#endif // QPACKAGEKITUPDATERBACKEND_H
