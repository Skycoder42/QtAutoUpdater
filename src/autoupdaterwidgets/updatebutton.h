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

	//! The file of the animation to be shown
	Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile NOTIFY animationFileChanged)
	//! Specifies whether a result should be shown within the button or not
	Q_PROPERTY(bool showResult READ isShowingResult WRITE setShowResult NOTIFY showResultChanged)
	Q_PROPERTY(QtAutoUpdater::Updater* updater READ updater WRITE setUpdater NOTIFY updaterChanged)

public:
	//! Creates a new update button to place in your GUI
	explicit UpdateButton(QWidget *parent = nullptr);
	explicit UpdateButton(Updater *updater, QWidget *parent = nullptr);

	//! @readAcFn{UpdateButton::animationFile}
	QString animationFile() const;
	//! @readAcFn{UpdateButton::showResult}
	bool isShowingResult() const;
	//! @readAcFn{UpdateButton::updater}
	Updater* updater() const;

public Q_SLOTS:
	//! Rests the buttons visual state
	void resetState();

	//! @writeAcFn{UpdateButton::animationFile}
	void setAnimationFile(const QString &animationFile, int speed = 100);
	//! @writeAcFn{UpdateButton::animationFile}
	void setAnimationDevice(QIODevice *animationDevice, int speed = 100);
	//! @resetAcFn{UpdateButton::animationFile}
	void resetAnimationFile();
	//! @writeAcFn{UpdateButton::showResult}
	void setShowResult(bool showResult);
	//! @writeAcFn{UpdateButton::updater}
	void setUpdater(Updater* updater);

Q_SIGNALS:
	//! @notifyAcFn{UpdateButton::updater}
	void updaterChanged(QtAutoUpdater::Updater* updater, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::showResult}
	void showResultChanged(bool showResult, QPrivateSignal);
	//! @notifyAcFn{UpdateButton::animationFile}
	void animationFileChanged(const QString &animationFile, QPrivateSignal);

private:
	Q_DECLARE_PRIVATE(UpdateButton)

	Q_PRIVATE_SLOT(d_func(), void _q_changeUpdaterState(Updater::State))
	Q_PRIVATE_SLOT(d_func(), void _q_updaterDestroyed())
};

}

#endif // QTAUTOUPDATER_UPDATEBUTTON_H
