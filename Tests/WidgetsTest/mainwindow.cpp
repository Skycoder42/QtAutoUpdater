#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	controller(new QtAutoUpdater::UpdateController(this))
{
	ui->setupUi(this);
	this->statusBar()->showMessage("not running");

	connect(this->controller, &QtAutoUpdater::UpdateController::runningChanged, this, [this](bool running){
		this->statusBar()->showMessage(running ? "running" : "not running");
	});

	QSettings settings("./set.ini", QSettings::IniFormat);
	this->ui->maintenanceToolLineEdit->setText(settings.value("path").toString());
	this->ui->displayLevelComboBox->setCurrentIndex((QtAutoUpdater::UpdateController::DisplayLevel)settings.value("level", QtAutoUpdater::UpdateController::AskLevel).toInt());
}

MainWindow::~MainWindow()
{
	QSettings settings("./set.ini", QSettings::IniFormat);
	settings.setValue("path", this->ui->maintenanceToolLineEdit->text());
	settings.setValue("level", this->ui->displayLevelComboBox->currentIndex());
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
												QStringLiteral("Applications (*.app);;All Files (*)")
											#elif defined(Q_OS_UNIX)
												QStringLiteral("Executables (*)")
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

void MainWindow::on_cancelButton_clicked()
{
	qDebug() << "cancel controller:" << this->controller->cancelUpdate();
}
