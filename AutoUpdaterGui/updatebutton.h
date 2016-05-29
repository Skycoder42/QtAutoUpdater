#ifndef UPDATEBUTTON_H
#define UPDATEBUTTON_H

#include <QWidget>
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
		Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile NOTIFY animationFileChanged)
		//! Specifies whether a result should be shown within the button or not
		Q_PROPERTY(bool showResult READ isShowingResult WRITE setShowResult NOTIFY showResultChanged)
		//! The display level to start the controller with
		Q_PROPERTY(UpdateController::DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel NOTIFY displayLevelChanged)

	public:
		//! Creates a new update button to place in your GUI
		explicit UpdateButton(UpdateController *controller, QWidget *parent = 0);
		//! Destructor
		~UpdateButton();

		//! READ-Accessor for UpdateButton::animationFile
		QString animationFile() const;
		//! READ-Accessor for UpdateButton::showResult
		bool isShowingResult() const;
		//! READ-Accessor for UpdateButton::displayLevel
		UpdateController::DisplayLevel displayLevel() const;

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

	signals:
		//! NOTIFY-Accessor for UpdateButton::animationFile
		void animationFileChanged(QString animationFile);
		//! NOTIFY-Accessor for UpdateButton::showResult
		void showResultChanged(bool showResult);
		//! NOTIFY-Accessor for UpdateButton::displayLevel
		void displayLevelChanged(UpdateController::DisplayLevel displayLevel);

	private slots:
		void startUpdate();
		void changeUpdaterState(bool isRunning);
		void updatesReady(bool hasUpdate, bool);

	private:
		UpdateButtonPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateButton)
	};
}

#endif // UPDATEBUTTON_H
