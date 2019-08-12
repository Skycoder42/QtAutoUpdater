#ifndef QTAUTOUPDATER_UPDATERPLUGIN_H
#define QTAUTOUPDATER_UPDATERPLUGIN_H

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updaterbackend.h"

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT UpdaterPlugin
{
	Q_DISABLE_COPY(UpdaterPlugin)

public:
	UpdaterPlugin();
	virtual ~UpdaterPlugin();

	virtual UpdaterBackend *createInstance(const QString &type, QObject *parent) = 0;
};

}

#define QtAutoUpdater_UpdaterPlugin_iid "de.skycoder42.qtautoupdater.core.UpdaterPlugin"

Q_DECLARE_INTERFACE(QtAutoUpdater::UpdaterPlugin, QtAutoUpdater_UpdaterPlugin_iid)

#endif // QTAUTOUPDATER_UPDATERPLUGIN_H
