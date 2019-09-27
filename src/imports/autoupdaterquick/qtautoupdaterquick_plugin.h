#ifndef QTAUTOUPDATERQUICK_PLUGIN_H
#define QTAUTOUPDATERQUICK_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

namespace QtAutoUpdater {

class QtAutoUpdaterQuickDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	explicit QtAutoUpdaterQuickDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

}

#endif // QTAUTOUPDATERQUICK_PLUGIN_H
