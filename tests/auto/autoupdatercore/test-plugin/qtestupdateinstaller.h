#ifndef QTESTUPDATEINSTALLER_H
#define QTESTUPDATEINSTALLER_H

#include <QtCore/QTimer>

#include <QtAutoUpdaterCore/UpdateInstaller>
#include <QtAutoUpdaterCore/UpdaterBackend>

class QTestUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	explicit QTestUpdateInstaller(QtAutoUpdater::UpdaterBackend::IConfigReader *config, QObject *parent = nullptr);

	Features features() const override;

public Q_SLOTS:
	void eulaHandled(const QVariant &id, bool accepted) override;
	void cancelInstall() override;

protected:
	void startInstallImpl() override;

private Q_SLOTS:
	void doInstallStep();

private:
	QtAutoUpdater::UpdaterBackend::IConfigReader *_config;
	int _openEulas = 0;

	QTimer *_updateTimer;
	int _currentIndex = 0;
	int _currentProgress = 0;

	void doInstall();
};

#endif // QTESTUPDATEINSTALLER_H
