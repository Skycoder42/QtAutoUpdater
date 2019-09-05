#ifndef QTAUTOUPDATER_UPDATECONTROLLER_H
#define QTAUTOUPDATER_UPDATECONTROLLER_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

#include <QtAutoUpdaterCore/updater.h>

#include <QtWidgets/qaction.h>

#include "QtAutoUpdaterWidgets/qtautoupdaterwidgets_global.h"

namespace QtAutoUpdater
{

class Updater;
class UpdateControllerPrivate;
//! A class to show a controlled update GUI to the user
class Q_AUTOUPDATERWIDGETS_EXPORT UpdateController : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QtAutoUpdater::Updater* updater READ updater WRITE setUpdater NOTIFY updaterChanged)
	Q_PROPERTY(DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel NOTIFY displayLevelChanged)
	Q_PROPERTY(QString desktopFileName READ desktopFileName WRITE setDesktopFileName NOTIFY desktopFileNameChanged)

public:
	//! Defines the different display-levels of the dialog
	enum class DisplayLevel {
		Automatic = 0,//!< The lowest level. Nothing will be displayed at all. The programm will be auto-closed.
		Exit = 1,/*!< The whole updating works completly automatically without displaying anything. Only
					   *   a notification that updates are ready to install will be shown if updates are available.
					   */
		Info = 2,//!< Will show information about updates if available, nothing otherwise.
		ExtendedInfo = 3,//!< Will show information about the update result, for both cases, updates and no updates.
		Progress = 4,//!< Shows a (modal) progress dialog while checking for updates.
		Ask = 5//!< The highest level. Will ask the user if he wants to check for updates before actually checking.
	};
	Q_ENUM(DisplayLevel)

	explicit UpdateController(QObject *parent = nullptr);
	explicit UpdateController(QWidget *parentWindow);
	//! Constructs a new controller with an explicitly set path and a parent. Will be application modal
	explicit UpdateController(Updater *updater, QObject *parent = nullptr);
	//! Constructs a new controller with an explicitly set path and a parent. Will modal to the parent window
	explicit UpdateController(Updater *updater, QWidget *parentWindow);
	~UpdateController() override;

	//! Create a QAction to start this controller from
	static QAction *createUpdateAction(Updater *updater, QObject *parent);

	QWidget* parentWindow() const;
	//! @readAcFn{UpdateController::running}
	DisplayLevel displayLevel() const;
	//! @readAcFn{UpdateController::desktopFileName}
	QString desktopFileName() const;
	//! @readAcFn{UpdateController::updater}
	Updater *updater() const;

public Q_SLOTS:
	//! @writeAcFn{UpdateController::displayLevel}
	void setDisplayLevel(DisplayLevel displayLevel);
	//! @writeAcFn{UpdateController::desktopFileName}
	void setDesktopFileName(QString desktopFileName);
	//! @writeAcFn{UpdateController::updater}
	void setUpdater(QtAutoUpdater::Updater* updater);


	bool start();
	//! Starts the controller with the specified display level
	bool start(DisplayLevel displayLevel);
	//! Tries to cancel the controllers update
	bool cancelUpdate(int maxDelay = 3000);

Q_SIGNALS:
	//! @notifyAcFn{UpdateController::displayLevel}
	void displayLevelChanged(DisplayLevel displayLevel, QPrivateSignal);
	//! @notifyAcFn{UpdateController::desktopFileName}
	void desktopFileNameChanged(const QString &desktopFileName, QPrivateSignal);
	//! @notifyAcFn{UpdateController::updater}
	void updaterChanged(QtAutoUpdater::Updater* updater, QPrivateSignal);

private:
	Q_DECLARE_PRIVATE(UpdateController)
	Q_DISABLE_COPY(UpdateController)

	Q_PRIVATE_SLOT(d_func(), void _q_updaterStateChanged(Updater::State))
	Q_PRIVATE_SLOT(d_func(), void _q_showInstaller(UpdateInstaller *))
	Q_PRIVATE_SLOT(d_func(), void _q_updaterDestroyed())
};

}

#endif // QTAUTOUPDATER_UPDATECONTROLLER_H
