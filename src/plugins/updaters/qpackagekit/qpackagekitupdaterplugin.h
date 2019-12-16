#ifndef QPACKAGEKITUPDATERPLUGIN_H
#define QPACKAGEKITUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QPackageKitUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qpackagekit.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QPackageKitUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QPACKAGEKITUPDATERPLUGIN_H
