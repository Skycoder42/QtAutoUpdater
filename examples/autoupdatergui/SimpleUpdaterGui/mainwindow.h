#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtAutoUpdaterGui/UpdateController>
#include <QtAutoUpdaterGui/UpdateButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	QtAutoUpdater::UpdateController *controller;
	QtAutoUpdater::UpdateButton *updateButton;

private slots:
	void initializeUpdater();
	void checkUpdate();
};

#endif // MAINWINDOW_H
