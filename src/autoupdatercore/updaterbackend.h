#ifndef QTAUTOUPDATER_UPDATERBACKEND_H
#define QTAUTOUPDATER_UPDATERBACKEND_H

#include <optional>

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qloggingcategory.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updateinfo.h"

namespace QtAutoUpdater {

class UpdateInstaller;

class UpdaterBackendPrivate;
class Q_AUTOUPDATERCORE_EXPORT UpdaterBackend : public QObject
{
	Q_OBJECT

public:
	enum class Feature {
		CheckUpdates = 0x00,
		CheckProgress = 0x01,
		TriggerInstall = 0x02,
		ParallelInstall = 0x04,
		PerformInstall = 0x08 | ParallelInstall
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	class Q_AUTOUPDATERCORE_EXPORT IConfigReader
	{
		Q_DISABLE_COPY(IConfigReader)
	public:
		IConfigReader() = default;
		virtual ~IConfigReader() = default;

		virtual QString backend() const = 0;
		virtual std::optional<QVariant> value(const QString &key) const = 0;
		virtual QVariant value(const QString &key, const QVariant &defaultValue) const = 0;
	};

	QString key() const ;
	virtual Features features() const = 0;
	bool initialize(QScopedPointer<IConfigReader> &&config);

	virtual void checkForUpdates() = 0;
	virtual void abort(bool force) = 0;

	virtual bool triggerUpdates(const QList<UpdateInfo> &infos, bool track) = 0;
	virtual UpdateInstaller *createInstaller() = 0;

Q_SIGNALS:
	void checkProgress(double percent, const QString &status);
	void checkDone(bool success, const QList<UpdateInfo> &updates = {});
	void triggerInstallDone(bool success);

protected:
	explicit UpdaterBackend(QString &&key, QObject *parent = nullptr);
	explicit UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent = nullptr);

	IConfigReader *config() const;
	const QLoggingCategory &logCat() const;

	virtual bool initialize() = 0;

private:
	friend class Updater;
	Q_DECLARE_PRIVATE(UpdaterBackend)
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdaterBackend::Features)

#endif // QTAUTOUPDATER_UPDATERBACKEND_H
