#ifndef QTAUTOUPDATERCORE_PLUGIN_H
#define QTAUTOUPDATERCORE_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

namespace QtAutoUpdater {

class QtAutoUpdaterCoreDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	explicit QtAutoUpdaterCoreDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

}

#endif // QTAUTOUPDATERCORE_PLUGIN_H
