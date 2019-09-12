#ifndef QPLAYSTOREUPDATERPLUGIN_H
#define QPLAYSTOREUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QPlayStoreUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qplaystore.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QPlayStoreUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QPLAYSTOREUPDATERPLUGIN_H
