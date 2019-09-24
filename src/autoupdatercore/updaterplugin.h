#ifndef QTAUTOUPDATER_UPDATERPLUGIN_H
#define QTAUTOUPDATER_UPDATERPLUGIN_H

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updaterbackend.h"

namespace QtAutoUpdater {

//! The plugin interface to implement to create an updater plugin
class Q_AUTOUPDATERCORE_EXPORT UpdaterPlugin
{
	Q_DISABLE_COPY(UpdaterPlugin)

public:
	UpdaterPlugin();
	virtual ~UpdaterPlugin();

	//! Create a new instance of that plugins updater backend
	virtual UpdaterBackend *create(QString key, QObject *parent) = 0;
};

}

//! The IID of the QtAutoUpdater::UpdaterPlugin interface
#define QtAutoUpdater_UpdaterPlugin_iid "de.skycoder42.qtautoupdater.core.UpdaterPlugin"

Q_DECLARE_INTERFACE(QtAutoUpdater::UpdaterPlugin, QtAutoUpdater_UpdaterPlugin_iid)

//! @file updaterplugin.h The QtAutoUpdater::UpdaterPlugin header
#endif // QTAUTOUPDATER_UPDATERPLUGIN_H
