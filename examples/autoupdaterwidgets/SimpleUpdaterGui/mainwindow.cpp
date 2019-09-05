#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	initializeUpdater();	//Initialize the updater

	controller->start(QtAutoUpdater::UpdateController::DisplayLevel::Info);	//Search for updates. Display a message if updates found, otherwise do nothing
}

MainWindow::~MainWindow()
{
	delete ui;
}

//Initialize the updater
void MainWindow::initializeUpdater()
{
	auto updater = QtAutoUpdater::Updater::create(QStringLiteral(CONFIG_PATH "/example.conf"), qApp);
	Q_ASSERT(updater);
	controller = new QtAutoUpdater::UpdateController(updater, this);
	ui->updateButton->setUpdater(controller->updater());
	auto action = QtAutoUpdater::UpdateController::createUpdateAction(updater, this);
	ui->menuAbout->addAction(action);
	ui->mainToolBar->addAction(action);
}
