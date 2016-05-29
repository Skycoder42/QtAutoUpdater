#ifndef UPDATEBUTTON_H
#define UPDATEBUTTON_H

#include <QWidget>
#include <QScopedPointer>
#include "updatecontroller.h"

namespace QtAutoUpdater
{
	//! @internal Private implementation
	class UpdateButtonPrivate;
	//! A simple button for update checks
	class UpdateButton : public QWidget
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

	public slots:
		//! Rests the buttons visual state
		void resetState();

		//! WRITE-Accessor for UpdateButton::animationFile
		void setAnimationFile(QString animationFile);
		//! WRITE-Accessor for UpdateButton::animationFile
		void setAnimationDevice(QIODevice *animationDevice);
		//! RESET-Accessor for UpdateButton::animationFile
		void resetAnimationFile();
		//! WRITE-Accessor for UpdateButton::showResult
		void setShowResult(bool showResult);
		//! WRITE-Accessor for UpdateButton::displayLevel
		void setDisplayLevel(UpdateController::DisplayLevel displayLevel);
		//! WRITE-Accessor for UpdateButton::controller
		bool setController(UpdateController* controller);

	signals:
		//! NOTIFY-Accessor for UpdateButton::controller
		void controllerChanged(UpdateController* controller);

	private slots:
		void startUpdate();
		void changeUpdaterState(bool isRunning);
		void updatesReady(bool hasUpdate, bool);
		void controllerDestroyed();

	private:
		QScopedPointer<UpdateButtonPrivate> d_ptr;
		Q_DECLARE_PRIVATE(UpdateButton)
		UpdateController* m_controller;
	};
}

#endif // UPDATEBUTTON_H
