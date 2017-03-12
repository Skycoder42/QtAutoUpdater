#ifndef QAUTOUPDATER_H
#define QAUTOUPDATER_H

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_AUTOUPDATER_LIB)
#	define Q_AUTOUPDATER_EXPORT Q_DECL_EXPORT
#else
#	define Q_AUTOUPDATER_EXPORT Q_DECL_IMPORT
#endif

#endif // QAUTOUPDATER_H