#ifndef QTAUTOUPDATER_ADMINAUTHORISER_H
#define QTAUTOUPDATER_ADMINAUTHORISER_H

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace QtAutoUpdater::AdminAuthoriser
{

Q_AUTOUPDATERCORE_EXPORT bool needsAdminPermission(const QString &program);
Q_AUTOUPDATERCORE_EXPORT bool executeAsAdmin(const QString &program,
											 const QStringList &arguments,
											 const QString &workingDir);

}

#endif // QTAUTOUPDATER_ADMINAUTHORISER_H
