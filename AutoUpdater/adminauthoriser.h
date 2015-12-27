#ifndef ADMINAUTHORISER_H
#define ADMINAUTHORISER_H

#include <QString>
#include <QStringList>

namespace QtAutoUpdater
{
	//! An interface to run programs with elevated rights
	class AdminAuthoriser
	{
	public:
		//! Destructor
		virtual ~AdminAuthoriser() {}
		//! Specifies whether this program already has elevated rights or not
		virtual bool hasAdminRights() = 0;
		//! Runs a program with the given arguments with elevated rights
		virtual bool executeAsAdmin(const QString &program, const QStringList &arguments) = 0;
	};
}

#endif // ADMINAUTHORISER_H
