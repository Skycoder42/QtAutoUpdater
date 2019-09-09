#ifndef QTAUTOUPDATER_UPDATERBACKEND_P_H
#define QTAUTOUPDATER_UPDATERBACKEND_P_H

#include "updaterbackend.h"

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT UpdaterBackendPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdaterBackend)

public:
	UpdaterBackendPrivate(QString &&pKey);

	QString key;
	QScopedPointer<UpdaterBackend::IConfigReader> config;
};

}

#endif // UPDATERBACKEND_P_H
