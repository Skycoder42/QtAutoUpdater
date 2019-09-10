#ifndef QCHOCOLATEYUPDATERBACKEND_H
#define QCHOCOLATEYUPDATERBACKEND_H

#include <QtCore/QProcess>
#include <QtCore/QLoggingCategory>

#include <QtAutoUpdaterCore/UpdaterBackend>

class QChocolateyUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	explicit QChocolateyUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
	void updaterError(QProcess::ProcessError procError);
	void installerState(QProcess::ProcessState state);

private:
	QProcess *_chocoProc = nullptr;
	QStringList _packages;

	void parseUpdates();
};

Q_DECLARE_LOGGING_CATEGORY(logChocoBackend)

#endif // QCHOCOLATEYUPDATERBACKEND_H
