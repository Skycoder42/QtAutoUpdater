#ifndef QHOMEBREWUPDATEINSTALLER_H
#define QHOMEBREWUPDATEINSTALLER_H

#include <QtCore/QProcess>

#include <QtAutoUpdaterCore/UpdateInstaller>
#include <QtAutoUpdaterCore/UpdaterBackend>

class QHomebrewUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	explicit QHomebrewUpdateInstaller(const QString &brewPath,
									  QtAutoUpdater::UpdaterBackend::IConfigReader *config,
									  QObject *parent = nullptr);

	Features features() const override;

public Q_SLOTS:
	void eulaHandled(const QVariant &id, bool accepted) override;
	void restartApplication() override;

protected:
	void startInstallImpl() override;

private Q_SLOTS:
	void stateChanged(QProcess::ProcessState state);

private:
	QtAutoUpdater::UpdaterBackend::IConfigReader *_config;
	QProcess *_installProcess;
};

Q_DECLARE_LOGGING_CATEGORY(logBrewInstaller)

#endif // QHOMEBREWUPDATEINSTALLER_H
