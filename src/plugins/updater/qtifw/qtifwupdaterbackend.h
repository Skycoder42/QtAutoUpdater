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
	bool initialize(const QVariantMap &arguments,
					QtAutoUpdater::AdminAuthoriser *authoriser) override;
	QtAutoUpdater::UpdateInstaller *installUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;

public Q_SLOTS:
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos) override;

private Q_SLOTS:
	void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
	void updaterError();

private:
	QtAutoUpdater::AdminAuthoriser *_authoriser = nullptr;
	QProcess *_process = nullptr;
	bool _silentUpdate = false;

	std::optional<QFileInfo> findMaintenanceTool(const QVariantMap &arguments);
	std::optional<QList<QtAutoUpdater::UpdateInfo>> parseUpdates();
};

#endif // QTIFWUPDATERBACKEND_H
