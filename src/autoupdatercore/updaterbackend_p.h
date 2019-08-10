#ifndef QTAUTOUPDATER_UPDATERBACKEND_P_H
#define QTAUTOUPDATER_UPDATERBACKEND_P_H

#include "updaterbackend.h"

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater {

struct UpdateInfoPrivate : public QSharedData
{
	QString name;
	QVersionNumber version;
	quint64 size = 0;
	QVariant identifier;
};

class UpdaterBackendPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdaterBackend)

public:
};

}

#endif // QTAUTOUPDATER_UPDATERBACKEND_P_H
