#ifndef QTAUTOUPDATER_UPDATER_P_H
#define QTAUTOUPDATER_UPDATER_P_H

#include "qtautoupdatercore_global.h"
#include "updater.h"
#include "simplescheduler_p.h"
#include "updaterbackend.h"

#include <QtCore/QLoggingCategory>

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERCORE_EXPORT UpdaterPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(Updater)

public:
	bool running = false;
	QList<UpdateInfo> updateInfos;
	QString errorMsg;

	SimpleScheduler *scheduler = nullptr;
	UpdaterBackend *backend = nullptr;

	bool runOnExit = false;
	QScopedPointer<AdminAuthoriser> adminAuth;

	static QSettings *findDefaultConfig();
	static Updater *createUpdater(UpdaterBackend::IConfigReader *config,
								  QObject *parent,
								  AdminAuthoriser *authoriser);

	void setBackend(UpdaterBackend *newBackend);
	void _q_appAboutToExit();
	void _q_checkDone(QList<UpdateInfo> updates);
	void _q_error(QString errorMessage);
};

Q_AUTOUPDATERCORE_EXPORT Q_DECLARE_LOGGING_CATEGORY(logQtAutoUpdater)

}

#endif // QTAUTOUPDATER_UPDATER_P_H
