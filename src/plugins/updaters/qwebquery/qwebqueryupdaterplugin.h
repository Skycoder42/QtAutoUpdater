#ifndef QWEBQUERYUPDATERPLUGIN_H
#define QWEBQUERYUPDATERPLUGIN_H

#include <QtCore/QObject>
#include <QtAutoUpdaterCore/UpdaterPlugin>

class QWebQueryUpdaterPlugin : public QObject, public QtAutoUpdater::UpdaterPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QtAutoUpdater_UpdaterPlugin_iid FILE "qwebquery.json")
	Q_INTERFACES(QtAutoUpdater::UpdaterPlugin)

public:
	explicit QWebQueryUpdaterPlugin(QObject *parent = nullptr);

	QtAutoUpdater::UpdaterBackend *create(QString key, QObject *parent) override;
};

#endif // QWEBQUERYUPDATERPLUGIN_H
