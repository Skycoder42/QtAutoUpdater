#ifndef QPLAYSTOREUPDATERBACKEND_H
#define QPLAYSTOREUPDATERBACKEND_H

#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtAndroidExtras/QAndroidJniObject>

class QPlayStoreUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	explicit QPlayStoreUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;

private:
	QAndroidJniObject _appUpdateManager;
};

#endif // QPLAYSTOREUPDATERBACKEND_H
