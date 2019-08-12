#ifndef QTAUTOUPDATER_UPDATERBACKEND_H
#define QTAUTOUPDATER_UPDATERBACKEND_H

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updateinfo.h"
#include "QtAutoUpdaterCore/adminauthoriser.h"

namespace QtAutoUpdater {

class UpdateInstaller;

class Q_AUTOUPDATERCORE_EXPORT UpdaterBackend : public QObject
{
	Q_OBJECT

public:
	enum class Feature {
		CheckUpdates = 0x00,
		CheckProgress = 0x01,
		TriggerInstall = 0x02,
		InstallNeedsExit = 0x04,
		InstallSelected = 0x08,
		PerformInstall = 0x10
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	explicit UpdaterBackend(QObject *parent = nullptr);

	virtual Features features() const = 0;
	virtual bool initialize(const QVariantMap &arguments,
							AdminAuthoriser *authoriser) = 0;

	virtual UpdateInstaller *installUpdates(const QList<UpdateInfo> &infos) = 0;

	virtual void checkForUpdates() = 0;
	virtual void abort(bool force) = 0;

	virtual bool triggerUpdates(const QList<UpdateInfo> &infos) = 0;

Q_SIGNALS:
	void checkDone(const QList<UpdateInfo> &updates);
	void error(const QString &errorMsg);
	void updateProgress(double percent, const QString &status);
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdaterBackend::Features)

#endif // QTAUTOUPDATER_UPDATERBACKEND_H
