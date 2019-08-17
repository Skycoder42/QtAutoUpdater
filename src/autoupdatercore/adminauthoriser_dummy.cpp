#include "adminauthoriser.h"
using namespace QtAutoUpdater;

bool AdminAuthoriser::needsAdminPermission(const QString &)
{
	return false;
}

bool AdminAuthoriser::executeAsAdmin(const QString &, const QStringList &, const QString &)
{
	return false;
}
