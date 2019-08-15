#ifndef QTIFWUPDATERPLUGIN_H
#define QTIFWUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QtIfwUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qtifw.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QtIfwUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QTIFWUPDATERPLUGIN_H
