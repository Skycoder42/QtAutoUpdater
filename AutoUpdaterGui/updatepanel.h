#ifndef UPDATEPANEL_H
#define UPDATEPANEL_H

#include <QWidget>
#include <QPointer>
#include <QMovie>
#include "updatecontroller.h"

namespace Ui {
	class UpdatePanel;
}

namespace QtAutoUpdater
{
	//TODO make public
	class UpdatePanel : public QWidget
	{
		Q_OBJECT

		Q_PROPERTY(QString animationFile READ animationFile WRITE setAnimationFile NOTIFY animationFileChanged)
		Q_PROPERTY(bool showResult READ showResult WRITE setShowResult NOTIFY showResultChanged)
		Q_PROPERTY(UpdateController::DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel NOTIFY displayLevelChanged)

	public:
		//! Creates a new update panel to place in your GUI
		explicit UpdatePanel(UpdateController *controller, QWidget *parent = 0);
		~UpdatePanel();

		QString animationFile() const;
		bool showResult() const;
		UpdateController::DisplayLevel displayLevel() const;

	public slots:
		void resetState();

		void setAnimationFile(QString animationFile);
		void setAnimationDevice(QIODevice *animationDevice);
		void setShowResult(bool showResult);
		void setDisplayLevel(UpdateController::DisplayLevel displayLevel);

	signals:
		void animationFileChanged(QString animationFile);
		void showResultChanged(bool showResult);
		void displayLevelChanged(UpdateController::DisplayLevel displayLevel);

	private slots:
		void startUpdate();
		void changeUpdaterState(bool isRunning);
		void updatesReady(bool hasUpdate, bool);

	private:
		QPointer<UpdateController> controller;
		Ui::UpdatePanel *ui;
		UpdateController::DisplayLevel level;
		QMovie *loadingGif;
		bool showRes;
	};
}

#endif // UPDATEPANEL_H
