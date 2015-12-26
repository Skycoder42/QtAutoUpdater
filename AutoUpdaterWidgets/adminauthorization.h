#ifndef ADMINAUTHORIZATION_H
#define ADMINAUTHORIZATION_H

#include "adminauthoriser.h"

namespace QtAutoUpdater
{
	class AdminAuthorization : public AdminAuthoriser
	{
	public:
		bool hasAdminRights() Q_DECL_OVERRIDE;
        bool executeAsAdmin(const QString &program, const QStringList &arguments, const QString &workingDir) Q_DECL_OVERRIDE;
    };
}

#endif // ADMINAUTHORIZATION_H
