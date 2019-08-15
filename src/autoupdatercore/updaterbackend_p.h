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
	QByteArray logCatStr;
	QLoggingCategory logCat;
	QScopedPointer<UpdaterBackend::IConfigReader> config;
	QScopedPointer<AdminAuthoriser> authoriser;
};

}

#endif // UPDATERBACKEND_P_H
