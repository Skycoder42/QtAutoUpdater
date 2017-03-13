#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <updatebutton.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	controller(nullptr),
	button(new QtAutoUpdater::UpdateButton(this))
{
	ui->setupUi(this);
	statusBar()->showMessage("not running");
	ui->scheduleUpdateDateTimeEdit->setDateTime(QDateTime::currentDateTime());
	ui->buttonLayout->addWidget(button);

	QSettings settings("./set.ini", QSettings::IniFormat);
	ui->maintenanceToolLineEdit->setText(settings.value("path").toString());
	ui->hasParentWindowCheckBox->setChecked(settings.value("hasParent", true).toBool());
	ui->displayLevelComboBox->setCurrentIndex((QtAutoUpdater::UpdateController::DisplayLevel)settings.value("level", QtAutoUpdater::UpdateController::ProgressLevel).toInt());
	ui->detailedInfoDialogCheckBox->setChecked(settings.value("detailed", true).toBool());
	ui->adminCheckBox->setChecked(settings.value("admin", true).toBool());
	ui->userChangecheckBox->setChecked(settings.value("adminChangable", true).toBool());
}

MainWindow::~MainWindow()
{
	QSettings settings("./set.ini", QSettings::IniFormat);
	settings.setValue("path", ui->maintenanceToolLineEdit->text());
	settings.setValue("hasParent", ui->hasParentWindowCheckBox->isChecked());
	settings.setValue("level", ui->displayLevelComboBox->currentIndex());
	settings.setValue("detailed", ui->detailedInfoDialogCheckBox->isChecked());
	settings.setValue("admin", ui->adminCheckBox->isChecked());
	settings.setValue("adminChangable", ui->userChangecheckBox->isChecked());
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
		ui->maintenanceToolLineEdit->setText(path);
}

void MainWindow::on_checkUpdatesButton_clicked()
{
	if(!controller->isRunning()) {
		controller->setRunAsAdmin(ui->adminCheckBox->isChecked(), ui->userChangecheckBox->isChecked());
		if(ui->scheduleUpdateDateCheckBox->isChecked()) {
			int id = controller->scheduleUpdate(ui->scheduleUpdateDateTimeEdit->dateTime(),
													  (QtAutoUpdater::UpdateController::DisplayLevel)ui->displayLevelComboBox->currentIndex());
			if(id)
				qDebug() << "update scheduled with id" << id << "to run at" << ui->scheduleUpdateDateTimeEdit->dateTime();
			else
				qDebug() << "failed to start controller at" << ui->scheduleUpdateDateTimeEdit->dateTime();
		} else
			qDebug() << "start controller:" << controller->start((QtAutoUpdater::UpdateController::DisplayLevel)ui->displayLevelComboBox->currentIndex());
	} else
		qDebug() << "start controller:" << false;
}

void MainWindow::on_cancelButton_clicked()
{
	qDebug() << "cancel controller:" << controller->cancelUpdate();
}

void MainWindow::on_activeBox_toggled(bool checked)
{
	if(checked) {
		if(ui->hasParentWindowCheckBox->isChecked())
			controller = new QtAutoUpdater::UpdateController(ui->maintenanceToolLineEdit->text(), this, qApp);
		else
			controller = new QtAutoUpdater::UpdateController(ui->maintenanceToolLineEdit->text(), qApp);
		controller->setDetailedUpdateInfo(ui->detailedInfoDialogCheckBox->isChecked());
		QAction *a = controller->createUpdateAction(this);
		a->setIconVisibleInMenu(false);
		ui->menuHelp->addAction(a);
		ui->mainToolBar->addAction(a);
		qDebug() << "detected runAsAdmin as:" << controller->runAsAdmin();
#ifdef Q_OS_OSX
		QMenu *dockMenu = new QMenu(this);
		QAction *action = controller->createUpdateAction(this);
		action->setMenuRole(QAction::NoRole);
		dockMenu->addAction(action);
		qt_mac_set_dock_menu(dockMenu);
#endif
		connect(controller, &QtAutoUpdater::UpdateController::runningChanged, this, [this](bool running){
			statusBar()->showMessage(running ? "running" : "not running");
		});
	} else {
		controller->deleteLater();
		controller = nullptr;
		statusBar()->showMessage("not running");
	}
	button->setController(controller);
}

void MainWindow::on_hasParentWindowCheckBox_clicked(bool checked)
{
	if(controller) {
		controller->setParentWindow(checked ? this : nullptr);
	}
}

void MainWindow::on_detailedInfoDialogCheckBox_clicked(bool checked)
{
	if(controller)
		controller->setDetailedUpdateInfo(checked);
}
