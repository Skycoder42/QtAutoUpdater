#ifndef QTIFWUPDATERBACKEND_H
#define QTIFWUPDATERBACKEND_H

#include <optional>

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QtAutoUpdaterCore/UpdaterBackend>

class QtIfwUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	explicit QtIfwUpdaterBackend(QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;
	QtAutoUpdater::UpdateInstaller *installUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
	void updaterError();

private:
	QScopedPointer<IConfigReader> _config;
	QtAutoUpdater::AdminAuthoriser *_authoriser = nullptr;
	QProcess *_process = nullptr;

	std::optional<QFileInfo> findMaintenanceTool();
	std::optional<QList<QtAutoUpdater::UpdateInfo>> parseUpdates();
};

#endif // QTIFWUPDATERBACKEND_H
