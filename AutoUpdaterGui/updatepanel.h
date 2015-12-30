#ifndef UPDATEPANEL_H
#define UPDATEPANEL_H

#include <QWidget>
#include <QPointer>
#include <QMovie>

namespace Ui {
	class UpdatePanel;
}

namespace QtAutoUpdater
{
	class UpdateController;
	class UpdatePanel : public QWidget
	{
		Q_OBJECT

	public:
		explicit UpdatePanel(UpdateController *controller, QWidget *parent = 0);
		~UpdatePanel();

	private slots:
		void startUpdate();
		void changeUpdaterState(bool isRunning);
		void updatesReady(bool hasUpdate, bool);

	private:
		QPointer<UpdateController> controller;
		Ui::UpdatePanel *ui;
		QMovie *loadingGif;
	};
}

#endif // UPDATEPANEL_H
