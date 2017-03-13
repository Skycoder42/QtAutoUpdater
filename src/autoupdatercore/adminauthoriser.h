#ifndef ADMINAUTHORISER_H
#define ADMINAUTHORISER_H

#include "QtAutoUpdaterCore/qautoupdatercore_global.h"

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace QtAutoUpdater
{

//! An interface to run programs with elevated rights
class Q_AUTOUPDATERCORE_EXPORT AdminAuthoriser
{
public:
	//! Virtual destructor
	virtual inline ~AdminAuthoriser() {}
	//! Tests whether this program already has elevated rights or not
	virtual bool hasAdminRights() = 0;
	//! Runs a program with the given arguments with elevated rights
	virtual bool executeAsAdmin(const QString &program, const QStringList &arguments) = 0;
};

}

#endif // ADMINAUTHORISER_H
