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

	Q_PROPERTY(QtAutoUpdater::Updater* updater READ updater WRITE setUpdater NOTIFY updaterChanged)
	Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)
	//! The file of the animation to be shown
	Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile NOTIFY animationFileChanged)

public:
	enum class ModeFlag {
		ShowResult = 0x01,
		ShowStatus = 0x02,
		AllowInstall = 0x04,
		AllowCancel = 0x08,

		ForceOnExit = (AllowInstall | 0x10),

		None = 0x00,
		AllowAndShowAll = (ShowResult | ShowStatus | AllowInstall | AllowCancel)
	};
	Q_DECLARE_FLAGS(Mode, ModeFlag)
	Q_FLAG(Mode)

	//! Creates a new update button to place in your GUI
	explicit UpdateButton(QWidget *parent = nullptr);
	explicit UpdateButton(Updater *updater, QWidget *parent = nullptr);

	//! @readAcFn{UpdateButton::updater}
	Updater* updater() const;
	//! @readAcFn{UpdateButton::mode}
	Mode mode() const;
	//! @readAcFn{UpdateButton::animationFile}
	QString animationFile() const;



public Q_SLOTS:
	//! Rests the buttons visual state
	void resetState();

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

Q_SIGNALS:
	//! @notifyAcFn{UpdateButton::updater}
	void updaterChanged(QtAutoUpdater::Updater* updater, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::mode}
	void modeChanged(Mode mode, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::animationFile}
	void animationFileChanged(const QString &animationFile, QPrivateSignal);

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
