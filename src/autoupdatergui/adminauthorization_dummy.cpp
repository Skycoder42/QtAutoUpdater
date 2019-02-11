#include "adminauthorization_p.h"
using namespace QtAutoUpdater;

bool AdminAuthorization::hasAdminRights()
{
	return false;
}

bool AdminAuthorization::executeAsAdmin(const QString &program, const QStringList &arguments)
{
	Q_UNUSED(program)
	Q_UNUSED(arguments)
	return false;
}
