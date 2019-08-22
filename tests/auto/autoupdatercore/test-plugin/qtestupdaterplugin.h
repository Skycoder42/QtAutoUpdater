#ifndef QTESTUPDATERPLUGIN_H
#define QTESTUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QTestUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "testplugin.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QTestUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QTESTUPDATERPLUGIN_H
