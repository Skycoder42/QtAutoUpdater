#ifndef ADMINAUTHORIZATION_H
#define ADMINAUTHORIZATION_H

#include "adminauthoriser.h"

namespace QtAutoUpdater
{
	class AdminAuthorization : public AdminAuthoriser
	{
	public:
		bool hasAdminRights() override;
		bool executeAsAdmin(const QString &program, const QStringList &arguments) override;
    };
}

#endif // ADMINAUTHORIZATION_H
