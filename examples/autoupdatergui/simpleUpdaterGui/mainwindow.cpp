#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    updateButton(new QtAutoUpdater::UpdateButton(this)) //add this for the updater
{
    ui->setupUi(this);

    updateButton->setVisible(false); //The updateButton is not visible
    initializeUpdater();	//Initialize the updater
	
    controller->start(QtAutoUpdater::UpdateController::InfoLevel);	//Search for updates. Display a message if updates found, otherwise do nothing

    //Connect the checkUpdateButton button to the checkUpdate method (starts the update check process)
    connect(ui->checkUpdateButton, &QAbstractButton::clicked, this, &MainWindow::checkUpdate);

    //Connect the actionCheck_for_updates_2 to the method (starts the update check process)
    connect(ui->actionCheck_for_updates_2, &QAction::triggered, this, &MainWindow::checkUpdate);
}

MainWindow::~MainWindow()
{
    delete controller;
    delete updateButton;
    delete ui;
}

//Initialize the updater
void MainWindow::initializeUpdater()
{
    controller = new QtAutoUpdater::UpdateController(QStringLiteral("maintenancetool.exe"), qApp);	//Updater app name
    if(ui->checkBox_showDetailledUpdateInformations->isChecked()) controller->setDetailedUpdateInfo(true);  //If checkbox is checked, show detailled update infos
    else controller->setDetailedUpdateInfo(false);

    updateButton->setController(controller);
}

//Starts update check process
void MainWindow::checkUpdate()
{
    if(ui->checkBox_showDetailledUpdateInformations->isChecked()) controller->setDetailedUpdateInfo(true);  //If checkbox is checked, show detailled update infos
    else controller->setDetailedUpdateInfo(false);
    controller->start(QtAutoUpdater::UpdateController::ProgressLevel);	//Check for updates. Displays a progress bar when searching
}
