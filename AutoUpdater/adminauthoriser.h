#ifndef ADMINAUTHORISER_H
#define ADMINAUTHORISER_H

#include <QString>
#include <QStringList>

namespace QtAutoUpdater
{
	class AdminAuthoriser
	{
	public:
		virtual ~AdminAuthoriser() {}
		virtual bool hasAdminRights() = 0;
		virtual bool executeAsAdmin(const QString &program, const QStringList &arguments, const QString &workingDir) = 0;
	};
}

#endif // ADMINAUTHORISER_H
