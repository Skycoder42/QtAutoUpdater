#ifndef QTAUTOUPDATER_UPDATEINSTALLER_H
#define QTAUTOUPDATER_UPDATEINSTALLER_H

#include <QtCore/qobject.h>
#include <QtCore/quuid.h>
#include <QtCore/qabstractitemmodel.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updateinfo.h"

namespace QtAutoUpdater {

class ComponentModel;

class UpdateInstallerPrivate;
//! An interface to implement and consume custom in-process installer
class Q_AUTOUPDATERCORE_EXPORT UpdateInstaller : public QObject
{
	Q_OBJECT

	//! Holds the features this installer supports
	Q_PROPERTY(QtAutoUpdater::UpdateInstaller::Features features READ features CONSTANT)
	//! The components to be installed
	Q_PROPERTY(QList<QtAutoUpdater::UpdateInfo> components READ components WRITE setComponents NOTIFY componentsChanged)
	//! An item model to let the user interactively select components to be installed
	Q_PROPERTY(QAbstractItemModel* componentModel READ componentModel CONSTANT)
	//! An item model showing the progress and status of each update component being installed
	Q_PROPERTY(QAbstractItemModel* progressModel READ progressModel CONSTANT)

public:
	//! Various features an installer may or may not support
	enum class Feature {
		None = 0x00, //!< The installer does not support any of the optional features
		SelectComponents = 0x01, //!< The installer allows the user to interactively select components
		DetailedProgress = 0x02, //!< The installer provides progress information for each component beeing installed.
		CanCancel = 0x04 //!< The installation can be canceled
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	//! @readAcFn{UpdateInstaller::features}
	virtual Features features() const = 0;
	//! @readAcFn{UpdateInstaller::components}
	virtual QList<UpdateInfo> components() const;
	//! @readAcFn{UpdateInstaller::componentModel}
	virtual QAbstractItemModel *componentModel() const;
	//! @readAcFn{UpdateInstaller::progressModel}
	virtual QAbstractItemModel *progressModel() const;

public Q_SLOTS:
	//! Is called to start the actual installation with the current components
	virtual void startInstall();
	//! Is called to cancel an ongoing installation
	virtual void cancelInstall();
	//! Is called to report the result of a user accepting or rejecting a required EULA
	virtual void eulaHandled(const QVariant &id, bool accepted) = 0;

	//! Is called to restart the application
	virtual void restartApplication();

	//! @writeAcFn{UpdateInstaller::components}
	virtual void setComponents(QList<QtAutoUpdater::UpdateInfo> components);

Q_SIGNALS:
	//! Is emitted to report the global, overall progress and status of the installation
	void updateGlobalProgress(double progress, const QString &status = {});
	//! Is emitted to report the progress and status of a single component beeing updated
	void updateComponentProgress(const QVariant &id, double progress, const QString &status = {});
	//! Is emitted if a EULA must be shown to the user
	void showEula(const QVariant &id, const QString &htmlText, bool required);
	//! Is emitted when the installation has succeeded
	void installSucceeded(bool shouldRestart);
	//! Is emitted when the installation has failed
	void installFailed(const QString &errorMessage);

	//! @notifyAcFn{UpdateInstaller::components}
	void componentsChanged();

protected:
	//! Default constructor
	explicit UpdateInstaller(QObject *parent = nullptr);
	//! @private
	explicit UpdateInstaller(UpdateInstallerPrivate &dd, QObject *parent = nullptr);

	//! Internal method to actually start the installation
	virtual void startInstallImpl() = 0;

	//! Enables or disables a component for installation
	virtual void setComponentEnabled(const QVariant &id, bool enabled);

private:
	Q_DECLARE_PRIVATE(UpdateInstaller)
	Q_DISABLE_COPY(UpdateInstaller)
	friend class QtAutoUpdater::ComponentModel;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdateInstaller::Features)
Q_DECLARE_METATYPE(QtAutoUpdater::UpdateInstaller*)

#endif // QTAUTOUPDATER_UPDATEINSTALLER_H
