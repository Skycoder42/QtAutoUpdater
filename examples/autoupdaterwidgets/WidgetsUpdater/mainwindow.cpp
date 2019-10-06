#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow{parent},
	ui{new Ui::MainWindow},
	button{new QtAutoUpdater::UpdateButton{this}}
{
	ui->setupUi(this);
	statusBar()->showMessage(tr("not running"));
	ui->scheduleUpdateDateTimeEdit->setDateTime(QDateTime::currentDateTime());
	ui->buttonLayout->addWidget(button);

	QSettings settings;
	ui->configPathLineEdit->setText(settings.value(QStringLiteral("path"), QStringLiteral(EXAMPLE_PATH)).toString());
	ui->hasParentWindowCheckBox->setChecked(settings.value(QStringLiteral("hasParent"), true).toBool());
	ui->displayLevelComboBox->setCurrentIndex(settings.value(QStringLiteral("level")).toInt());
	ui->detailedInfosCheckBox->setChecked(settings.value(QStringLiteral("details"), false).toBool());
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue(QStringLiteral("path"), ui->configPathLineEdit->text());
	settings.setValue(QStringLiteral("hasParent"), ui->hasParentWindowCheckBox->isChecked());
	settings.setValue(QStringLiteral("level"), ui->displayLevelComboBox->currentIndex());
	settings.setValue(QStringLiteral("details"), ui->detailedInfosCheckBox->isChecked());
	delete ui;
}

void MainWindow::on_configPathToolButton_clicked()
{
	const auto path = QFileDialog::getOpenFileName(this,
												   tr("Configuration file path"),
												   QDir::currentPath(),
												   tr("Updater configuration file (*.conf *.ini)"));
	if (!path.isEmpty())
		ui->configPathLineEdit->setText(path);
}

void MainWindow::on_checkUpdatesButton_clicked()
{
	if (ui->scheduleUpdateDateCheckBox->isChecked()) {
		int id = controller->updater()->scheduleUpdate(ui->scheduleUpdateDateTimeEdit->dateTime());
		if (id)
			qDebug() << "update scheduled with id" << id << "to run at" << ui->scheduleUpdateDateTimeEdit->dateTime();
		else
			qDebug() << "failed to start controller at" << ui->scheduleUpdateDateTimeEdit->dateTime();
	} else
		qDebug() << "start controller:" << controller->start();
}

void MainWindow::on_cancelButton_clicked()
{
	controller->updater()->abortUpdateCheck();
}

void MainWindow::on_activeBox_toggled(bool checked)
{
	if (controller) {
		controller->updater()->deleteLater();
		controller->deleteLater();
		controller = nullptr;
	}

	if (checked) {
		const auto updater = QtAutoUpdater::Updater::create(ui->configPathLineEdit->text(), qApp);
		if (!updater) {
			QMessageBox::critical(this,
								  tr("Configuration invalid"),
								  tr("The given configuration file either does not exist or is invalid!"));
			ui->activeBox->setChecked(false);
			return;
		}
		if(ui->hasParentWindowCheckBox->isChecked())
			controller = new QtAutoUpdater::UpdateController{updater, this};
		else
			controller = new QtAutoUpdater::UpdateController{updater};
		controller->setDesktopFileName(QStringLiteral("WidgetsUpdater"));
		controller->setDetailedUpdateInfo(ui->detailedInfosCheckBox->isChecked());
		controller->setDisplayLevel(static_cast<QtAutoUpdater::UpdateController::DisplayLevel>(ui->displayLevelComboBox->currentIndex()));
		auto action = QtAutoUpdater::UpdateController::createUpdateAction(updater, this);
		action->setIconVisibleInMenu(false);
		ui->menuHelp->addAction(action);
		ui->mainToolBar->addAction(action);

#ifdef Q_OS_OSX
		QMenu *dockMenu = new QMenu{this};
		auto dockAction = QtAutoUpdater::UpdateController::createUpdateAction(updater, dockMenu);
		dockAction->setMenuRole(QAction::NoRole);
		dockMenu->addAction(dockAction);
		qt_mac_set_dock_menu(dockMenu);
#endif
		connect(controller->updater(), &QtAutoUpdater::Updater::runningChanged, this, [this](bool running){
			statusBar()->showMessage(running ? tr("running") : tr("not running"));
		});
		button->setUpdater(controller->updater());
	} else {
		statusBar()->showMessage(tr("not running"));
	}
}

void MainWindow::on_displayLevelComboBox_currentIndexChanged(int index)
{
	if (controller)
		controller->setDisplayLevel(static_cast<QtAutoUpdater::UpdateController::DisplayLevel>(index));
}
