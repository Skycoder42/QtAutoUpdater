#include "adminauthoriser.h"
#include "updater_p.h"
using namespace QtAutoUpdater;

bool AdminAuthoriser::needsAdminPermission(const QString &)
{
	qCWarning(logUpdater) << "AdminAuthoriser::needsAdminPermission is not supported on this platform";
	return false;
}

bool AdminAuthoriser::executeAsAdmin(const QString &, const QStringList &, const QString &)
{
	qCWarning(logUpdater) << "AdminAuthoriser::executeAsAdmin is not supported on this platform";
	return false;
}
