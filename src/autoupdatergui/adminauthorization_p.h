#ifndef ADMINAUTHORIZATION_P_H
#define ADMINAUTHORIZATION_P_H

#include "qtautoupdatergui_global.h"
#include "adminauthoriser.h"

namespace QtAutoUpdater
{

class Q_AUTOUPDATERGUI_EXPORT AdminAuthorization : public AdminAuthoriser
{
public:
	bool hasAdminRights() override;
	bool executeAsAdmin(const QString &program, const QStringList &arguments) override;
};

}

#endif // ADMINAUTHORIZATION_P_H
