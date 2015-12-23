#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <updatecontroller.h>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_maintenanceToolButton_clicked();

	void on_checkUpdatesButton_clicked();

	void on_cancelButton_clicked();

private:
	Ui::MainWindow *ui;
	QtAutoUpdater::UpdateController *controller;
};

#endif // MAINWINDOW_H
