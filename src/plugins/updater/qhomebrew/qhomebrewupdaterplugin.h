#ifndef QHOMEBREWUPDATERPLUGIN_H
#define QHOMEBREWUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QHomebrewUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qhomebrew.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QHomebrewUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QHOMEBREWUPDATERPLUGIN_H
