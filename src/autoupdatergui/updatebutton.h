#ifndef UPDATEBUTTON_H
#define UPDATEBUTTON_H

#include "QtAutoUpdaterGui/qautoupdatergui_global.h"
#include "QtAutoUpdaterGui/updatecontroller.h"

#include <QtCore/qscopedpointer.h>
#include <QtWidgets/qwidget.h>

namespace QtAutoUpdater
{

class UpdateButtonPrivate;
//! A simple button for update checks
class Q_AUTOUPDATERGUI_EXPORT UpdateButton : public QWidget
{
	Q_OBJECT

	//! The file of the animation to be shown
	Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile)
	//! Specifies whether a result should be shown within the button or not
	Q_PROPERTY(bool showResult READ isShowingResult WRITE setShowResult)
	//! The display level to start the controller with
	Q_PROPERTY(UpdateController::DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel)
	//! The update controller this button works with
	Q_PROPERTY(UpdateController* controller READ controller WRITE setController NOTIFY controllerChanged)

public:
	//! Creates a new update button to place in your GUI
	explicit UpdateButton(QWidget *parent = nullptr, UpdateController *controller = nullptr);
	//! Destructor
	~UpdateButton();

	//! READ-Accessor for UpdateButton::animationFile
	QString animationFile() const;
	//! READ-Accessor for UpdateButton::showResult
	bool isShowingResult() const;
	//! READ-Accessor for UpdateButton::displayLevel
	UpdateController::DisplayLevel displayLevel() const;
	//! READ-Accessor for UpdateButton::controller
	UpdateController* controller() const;

public Q_SLOTS:
	//! Rests the buttons visual state
	void resetState();

	//! WRITE-Accessor for UpdateButton::animationFile
	void setAnimationFile(QString animationFile, int speed = 100);
	//! WRITE-Accessor for UpdateButton::animationFile
	void setAnimationDevice(QIODevice *animationDevice, int speed = 100);
	//! RESET-Accessor for UpdateButton::animationFile
	void resetAnimationFile();
	//! WRITE-Accessor for UpdateButton::showResult
	void setShowResult(bool showResult);
	//! WRITE-Accessor for UpdateButton::displayLevel
	void setDisplayLevel(UpdateController::DisplayLevel displayLevel);
	//! WRITE-Accessor for UpdateButton::controller
	bool setController(UpdateController* controller);

Q_SIGNALS:
	//! NOTIFY-Accessor for UpdateButton::controller
	void controllerChanged(UpdateController* controller);

private Q_SLOTS:
	void startUpdate();
	void changeUpdaterState(bool isRunning);
	void updatesReady(bool hasUpdate, bool);
	void controllerDestroyed();

private:
	QScopedPointer<UpdateButtonPrivate> d_ptr;
	Q_DECLARE_PRIVATE(UpdateButton)
};

}

#endif // UPDATEBUTTON_H
