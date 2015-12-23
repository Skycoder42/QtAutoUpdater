#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	controller(new QtAutoUpdater::UpdateController(this))
{
	ui->setupUi(this);
	connect(this->controller, &QtAutoUpdater::UpdateController::runningChanged, this, [this](bool running){
		this->statusBar()->showMessage(running ? "running" : "not running");
	});
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_maintenanceToolButton_clicked()
{
	QString path = QFileDialog::getOpenFileName(this,
												tr("MaintenanceTool path"),
												QStringLiteral("/"),
											#if defined(Q_OS_WIN32)
												QStringLiteral("Executables (*.exe);;All Files (*)")
											#elif defined(Q_OS_OSX)
											#elif defined(Q_OS_UNIX)
											#endif
												);
	if(!path.isEmpty())
		this->ui->maintenanceToolLineEdit->setText(path);
}

void MainWindow::on_checkUpdatesButton_clicked()
{
	if(!this->controller->isRunning()) {
		this->controller->setMaintenanceToolPath(this->ui->maintenanceToolLineEdit->text());
		this->controller->setDisplayLevel((QtAutoUpdater::UpdateController::DisplayLevel)this->ui->displayLevelComboBox->currentIndex());
		qDebug() << "start controller:" << this->controller->start();
	}
}
