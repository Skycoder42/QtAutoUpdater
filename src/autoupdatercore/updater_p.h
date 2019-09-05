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
	using State = Updater::State;

	State state = State::NoUpdates;
	QList<UpdateInfo> updateInfos;

	SimpleScheduler *scheduler = nullptr;
	UpdaterBackend *backend = nullptr;

	bool runOnExit = false;

	static QSettings *findDefaultConfig();
	static Updater *createUpdater(UpdaterBackend::IConfigReader *config,
								  QObject *parent);

	void setBackend(UpdaterBackend *newBackend);
	void _q_appAboutToExit();
	void _q_checkDone(bool success, QList<UpdateInfo> updates);
	void _q_triggerInstallDone(bool success);
};

Q_DECLARE_LOGGING_CATEGORY(logUpdater)

}

#endif // QTAUTOUPDATER_UPDATER_P_H
