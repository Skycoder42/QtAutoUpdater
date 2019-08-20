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
	~MainWindow() override;

private:
	Ui::MainWindow *ui;
	QtAutoUpdater::UpdateController *controller = nullptr;

private slots:
	void initializeUpdater();
};

#endif // MAINWINDOW_H
