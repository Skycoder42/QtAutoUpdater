#ifndef QTAUTOUPDATER_UPDATEBUTTON_H
#define QTAUTOUPDATER_UPDATEBUTTON_H

#include "QtAutoUpdaterWidgets/qtautoupdaterwidgets_global.h"

#include <QtAutoUpdaterCore/updater.h>

#include <QtWidgets/qwidget.h>

namespace QtAutoUpdater
{

class UpdateButtonPrivate;
//! A simple button for update checks
class Q_AUTOUPDATERWIDGETS_EXPORT UpdateButton : public QWidget
{
	Q_OBJECT

	//! The updater the button will use to check for updates
	Q_PROPERTY(QtAutoUpdater::Updater* updater READ updater WRITE setUpdater NOTIFY updaterChanged)
	//! Holds the mode of the button
	Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
	//! The file of the animation to be shown
	Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile NOTIFY animationFileChanged)
	//! The installation mode to be used to install updates
	Q_PROPERTY(QtAutoUpdater::Updater::InstallMode installMode READ installMode WRITE setInstallMode NOTIFY installModeChanged)
	//! The installation scope to be used to install updates
	Q_PROPERTY(QtAutoUpdater::Updater::InstallScope installScope READ installScope WRITE setInstallScope NOTIFY installScopeChanged)

public:
	//! Defines various flags to specify how the button should behave
	enum class ModeFlag {
		ShowResult = 0x01,  //!< Show a result (e.g. "new updates") after a check completed next to the button
		ShowStatus = 0x02,  //!< Show the check status as provided via Updater::progressChanged instead of a simple "checking for updates"
		AllowInstall = 0x04,  //!< Allow the user to launch an installer via the button if updates are available
		AllowCancel = 0x08,  //!< Allow the user to cancel the update check

		None = 0x00,  //!< Value to enable none of these features
		AllowAndShowAll = (ShowResult | ShowStatus | AllowInstall | AllowCancel)  //!< Enables all features at once
	};
	Q_DECLARE_FLAGS(Mode, ModeFlag)
	Q_FLAG(Mode)

	//! Creates a new update button to place in your GUI
	explicit UpdateButton(QWidget *parent = nullptr);
	//! Creates a new update button with an updater to place in your GUI
	explicit UpdateButton(Updater *updater, QWidget *parent = nullptr);

	//! @readAcFn{UpdateButton::updater}
	Updater* updater() const;
	//! @readAcFn{UpdateButton::mode}
	Mode mode() const;
	//! @readAcFn{UpdateButton::animationFile}
	QString animationFile() const;
	//! @readAcFn{UpdateButton::installMode}
	Updater::InstallMode installMode() const;
	//! @readAcFn{UpdateButton::installScope}
	Updater::InstallScope installScope() const;

public Q_SLOTS:
	//! @writeAcFn{UpdateButton::updater}
	void setUpdater(Updater* updater);
	//! @writeAcFn{UpdateButton::mode}
	void setMode(Mode mode);
	//! @writeAcFn{UpdateButton::animationFile}
	void setAnimationFile(const QString &animationFile, int speed = 100);
	//! @writeAcFn{UpdateButton::animationFile}
	void setAnimationDevice(QIODevice *animationDevice, int speed = 100);
	//! @resetAcFn{UpdateButton::animationFile}
	void resetAnimationFile();
	//! @writeAcFn{UpdateButton::installMode}
	void setInstallMode(QtAutoUpdater::Updater::InstallMode installMode);
	//! @writeAcFn{UpdateButton::installScope}
	void setInstallScope(QtAutoUpdater::Updater::InstallScope installScope);

Q_SIGNALS:
	//! @notifyAcFn{UpdateButton::updater}
	void updaterChanged(QtAutoUpdater::Updater* updater, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::mode}
	void modeChanged(Mode mode, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::animationFile}
	void animationFileChanged(const QString &animationFile, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::installMode}
	void installModeChanged(QtAutoUpdater::Updater::InstallMode installMode, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::installScope}
	void installScopeChanged(QtAutoUpdater::Updater::InstallScope installScope, QPrivateSignal);

private:
	Q_DECLARE_PRIVATE(UpdateButton)

	Q_PRIVATE_SLOT(d_func(), void _q_changeUpdaterState(Updater::State))
	Q_PRIVATE_SLOT(d_func(), void _q_updateStatus(double, const QString &))
	Q_PRIVATE_SLOT(d_func(), void _q_updaterDestroyed())
	Q_PRIVATE_SLOT(d_func(), void _q_clicked())
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdateButton::Mode)

#endif // QTAUTOUPDATER_UPDATEBUTTON_H
