#ifndef QTAUTOUPDATER_ADMINAUTHORISER_H
#define QTAUTOUPDATER_ADMINAUTHORISER_H

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace QtAutoUpdater::AdminAuthoriser
{

//! Tries to find out if the given executable needs to be run as priviledged user
Q_AUTOUPDATERCORE_EXPORT bool needsAdminPermission(const QString &program);
//! Prompts the user to execute the given executable as priviledged user
Q_AUTOUPDATERCORE_EXPORT bool executeAsAdmin(const QString &program,
											 const QStringList &arguments,
											 const QString &workingDir);

}

#endif // QTAUTOUPDATER_ADMINAUTHORISER_H
