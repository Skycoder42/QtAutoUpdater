#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    controller(nullptr), //add this
    updateButton(new QtAutoUpdater::UpdateButton(this)) //and this for the updater
{
    ui->setupUi(this);

    updateButton->setVisible(false); //The updateButton is not visible
    initializeUpdater();	//Initialize the updater
	
    controller->start(QtAutoUpdater::UpdateController::InfoLevel);	//Search for updates. Display a message if updates found, otherwise do nothing

    connect(ui->checkUpdateButton,
            SIGNAL(clicked()),
            this,
            SLOT(checkUpdate())
            );	//Connect the checkUpdateButton button to the checkUpdate method (starts the update check process)

    connect(ui->actionCheck_for_updates_2,
            SIGNAL(triggered(bool)),
            this,
            SLOT(checkUpdate())
            );	//Connect the actionCheck_for_updates_2 to the method (starts the update check process)
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Initialize the updater
void MainWindow::initializeUpdater()
{
    controller = new QtAutoUpdater::UpdateController("maintenancetool.exe", qApp);	//Updater app name
    controller->setDetailedUpdateInfo(true);
    updateButton->setController(controller);
}

//Starts update check process
void MainWindow::checkUpdate()
{
    controller->start(QtAutoUpdater::UpdateController::ProgressLevel);	//Check for updates. Displays a progress bar when searching
}
