#ifndef UPDATEPANEL_H
#define UPDATEPANEL_H

#include <QWidget>
#include "updatecontroller.h"

namespace QtAutoUpdater
{
	//! @internal Private implementation
	class UpdatePanelPrivate;
	//! A simple button for update checks
	class UpdatePanel : public QWidget
	{
		Q_OBJECT

		//! The file of the animation to be shown
		Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile RESET resetAnimationFile NOTIFY animationFileChanged)
		//! Specifies whether a result should be shown within the panel or not
		Q_PROPERTY(bool showResult READ isShowingResult WRITE setShowResult NOTIFY showResultChanged)
		//! The display level to start the controller with
		Q_PROPERTY(UpdateController::DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel NOTIFY displayLevelChanged)

	public:
		//! Creates a new update panel to place in your GUI
		explicit UpdatePanel(UpdateController *controller, QWidget *parent = 0);
		//! Destructor
		~UpdatePanel();

		//! READ-Accessor for UpdatePanel::animationFile
		QString animationFile() const;
		//! READ-Accessor for UpdatePanel::showResult
		bool isShowingResult() const;
		//! READ-Accessor for UpdatePanel::displayLevel
		UpdateController::DisplayLevel displayLevel() const;

	public slots:
		//! Rests the panels visual state
		void resetState();

		//! WRITE-Accessor for UpdatePanel::animationFile
		void setAnimationFile(QString animationFile);
		//! WRITE-Accessor for UpdatePanel::animationFile
		void setAnimationDevice(QIODevice *animationDevice);
		//! RESET-Accessor for UpdatePanel::animationFile
		void resetAnimationFile();
		//! WRITE-Accessor for UpdatePanel::showResult
		void setShowResult(bool showResult);
		//! WRITE-Accessor for UpdatePanel::displayLevel
		void setDisplayLevel(UpdateController::DisplayLevel displayLevel);

	signals:
		//! NOTIFY-Accessor for UpdatePanel::animationFile
		void animationFileChanged(QString animationFile);
		//! NOTIFY-Accessor for UpdatePanel::showResult
		void showResultChanged(bool showResult);
		//! NOTIFY-Accessor for UpdatePanel::displayLevel
		void displayLevelChanged(UpdateController::DisplayLevel displayLevel);

	private slots:
		void startUpdate();
		void changeUpdaterState(bool isRunning);
		void updatesReady(bool hasUpdate, bool);

	private:
		UpdatePanelPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdatePanel)
	};
}

#endif // UPDATEPANEL_H
