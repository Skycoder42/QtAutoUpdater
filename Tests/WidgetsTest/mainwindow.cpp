#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <dialogmaster.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	controller(NULL)
{
	ui->setupUi(this);
	this->statusBar()->showMessage("not running");
	this->ui->scheduleUpdateDateTimeEdit->setDateTime(QDateTime::currentDateTime());

	QSettings settings("./set.ini", QSettings::IniFormat);
	this->ui->maintenanceToolLineEdit->setText(settings.value("path").toString());
	this->ui->hasParentWindowCheckBox->setChecked(settings.value("hasParent", true).toBool());
	this->ui->displayLevelComboBox->setCurrentIndex((QtAutoUpdater::UpdateController::DisplayLevel)settings.value("level", QtAutoUpdater::UpdateController::ProgressLevel).toInt());
	this->ui->detailedInfoDialogCheckBox->setChecked(settings.value("detailed", true).toBool());
	this->ui->adminCheckBox->setChecked(settings.value("admin", true).toBool());
	this->ui->userChangecheckBox->setChecked(settings.value("adminChangable", true).toBool());

	DialogMaster::TitleAsWindowTitle = false;
	DialogMaster::warning(Q_NULLPTR, "text", "title", "windowTitle");
}

MainWindow::~MainWindow()
{
	QSettings settings("./set.ini", QSettings::IniFormat);
	settings.setValue("path", this->ui->maintenanceToolLineEdit->text());
	settings.setValue("hasParent", this->ui->hasParentWindowCheckBox->isChecked());
	settings.setValue("level", this->ui->displayLevelComboBox->currentIndex());
	settings.setValue("detailed", this->ui->detailedInfoDialogCheckBox->isChecked());
	settings.setValue("admin", this->ui->adminCheckBox->isChecked());
	settings.setValue("adminChangable", this->ui->userChangecheckBox->isChecked());
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
		this->controller->setRunAsAdmin(this->ui->adminCheckBox->isChecked(), this->ui->userChangecheckBox->isChecked());
		if(this->ui->scheduleUpdateDateCheckBox->isChecked()) {
			int id = this->controller->scheduleUpdate(this->ui->scheduleUpdateDateTimeEdit->dateTime(),
													  (QtAutoUpdater::UpdateController::DisplayLevel)this->ui->displayLevelComboBox->currentIndex());
			if(id)
				qDebug() << "update scheduled with id" << id << "to run at" << this->ui->scheduleUpdateDateTimeEdit->dateTime();
			else
				qDebug() << "failed to start controller at" << this->ui->scheduleUpdateDateTimeEdit->dateTime();
		} else
			qDebug() << "start controller:" << this->controller->start((QtAutoUpdater::UpdateController::DisplayLevel)this->ui->displayLevelComboBox->currentIndex());
	} else
		qDebug() << "start controller:" << false;
}

void MainWindow::on_cancelButton_clicked()
{
	qDebug() << "cancel controller:" << this->controller->cancelUpdate();
}

void MainWindow::on_activeBox_toggled(bool checked)
{
	if(checked) {
		if(this->ui->hasParentWindowCheckBox->isChecked())
			this->controller = new QtAutoUpdater::UpdateController(this->ui->maintenanceToolLineEdit->text(), this, qApp);
		else
			this->controller = new QtAutoUpdater::UpdateController(this->ui->maintenanceToolLineEdit->text(), qApp);
		this->controller->setDetailedUpdateInfo(this->ui->detailedInfoDialogCheckBox->isChecked());
		QAction *a = this->controller->createUpdateAction(this);
		a->setIconVisibleInMenu(false);
		this->ui->menuHelp->addAction(a);
		this->ui->mainToolBar->addAction(a);
#ifdef Q_OS_OSX
		QMenu *dockMenu = new QMenu(this);
		QAction *action = this->controller->createUpdateAction(this);
		action->setMenuRole(QAction::NoRole);
		dockMenu->addAction(action);
		qt_mac_set_dock_menu(dockMenu);
#endif
		connect(this->controller, &QtAutoUpdater::UpdateController::runningChanged, this, [this](bool running){
			this->statusBar()->showMessage(running ? "running" : "not running");
		});
		this->ui->buttonLayout->addWidget(this->controller->createUpdatePanel(this));
	} else {
		this->controller->deleteLater();
		this->controller = NULL;
		this->statusBar()->showMessage("not running");
	}
}

void MainWindow::on_hasParentWindowCheckBox_clicked(bool checked)
{
	if(this->controller) {
		this->controller->setParentWindow(checked ? this : NULL);
	}
}

void MainWindow::on_detailedInfoDialogCheckBox_clicked(bool checked)
{
	if(this->controller)
		this->controller->setDetailedUpdateInfo(checked);
}
