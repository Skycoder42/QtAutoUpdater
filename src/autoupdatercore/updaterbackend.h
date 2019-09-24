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
//! An interface to be implemented by updater plugins to provide the updater functionalities
class Q_AUTOUPDATERCORE_EXPORT UpdaterBackend : public QObject
{
	Q_OBJECT

public:
	//! Various features a backend may or may not support
	enum class Feature {
		CheckUpdates = 0x00, //!< The backend can check for updates. Must be supported by all backends
		CheckProgress = 0x01, //!< When checking for updates, the backend reports a detailed progress
		TriggerInstall = 0x02, //!< The backend can start an installation application if the primary application exits
		ParallelTrigger = (TriggerInstall | 0x04), //!< The backend can start an installation application running parallel to the primary application
		PerformInstall = 0x08  //!< The backend can create an UpdateInstaller to perform an update internally.
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	//! A helper interface to allow backend generic access to a backends configuration
	class Q_AUTOUPDATERCORE_EXPORT IConfigReader
	{
		Q_DISABLE_COPY(IConfigReader)
	public:
		IConfigReader() = default;
		virtual ~IConfigReader() = default;

		//! Returns the key of the backend to be loaded
		virtual QString backend() const = 0;
		//! Returns a configuration value, but only if it exists
		virtual std::optional<QVariant> value(const QString &key) const = 0;
		//! Returns a configuration value if it exists, the default value otherwise
		virtual QVariant value(const QString &key, const QVariant &defaultValue) const = 0;
	};

	//! Holds the variant map key and display name of the secondary update information
	using SecondaryInfo = std::optional<std::pair<QString, QString>>;  // (key, display-name)

	//! Returns the key of this backend
	Q_INVOKABLE QString key() const;
	//! Returns the supported features
	virtual Features features() const = 0;
	//! Returns the secondary update info
	virtual SecondaryInfo secondaryInfo() const;
	//! Initializes the backend with the given configuration
	bool initialize(QScopedPointer<IConfigReader> &&config);

	//! Starts an update check
	virtual void checkForUpdates() = 0;
	//! Aborts an update check
	virtual void abort(bool force) = 0;

	//! Launches an installer application to install updates
	virtual bool triggerUpdates(const QList<UpdateInfo> &infos, bool track) = 0;
	//! Creates an update installer to internally install updates
	virtual UpdateInstaller *createInstaller() = 0;

	//! Helper function to convert a variant value into a string list
	static QStringList readStringList(const QVariant &value, QChar listSeperator = QLatin1Char(','));

Q_SIGNALS:
	//! Should be emitted when the check progress changes
	void checkProgress(double percent, const QString &status);
	//! Should be emitted when the update check completed
	void checkDone(bool success, const QList<QtAutoUpdater::UpdateInfo> &updates = {});
	//! Should be emitted when a tracked triggered installation finished
	void triggerInstallDone(bool success);

protected:
	//! Constructor using the backends key and a parent
	explicit UpdaterBackend(QString &&key, QObject *parent = nullptr);
	//! @private
	explicit UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent = nullptr);

	//! Returns the configuration for this backend instance
	IConfigReader *config() const;

	//! Internally initializes the backend
	virtual bool initialize() = 0;

private:
	friend class Updater;
	Q_DECLARE_PRIVATE(UpdaterBackend)
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdaterBackend::Features)

#endif // QTAUTOUPDATER_UPDATERBACKEND_H
