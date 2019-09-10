#ifndef QCHOCOLATEYUPDATERPLUGIN_H
#define QCHOCOLATEYUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QChocolateyUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qchocolatey.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QChocolateyUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QCHOCOLATEYUPDATERPLUGIN_H
