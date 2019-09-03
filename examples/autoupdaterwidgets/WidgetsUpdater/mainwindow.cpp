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

	QSettings settings{QStringLiteral("./settings.ini"), QSettings::IniFormat};
	ui->configPathLineEdit->setText(settings.value(QStringLiteral("path")).toString());
	ui->hasParentWindowCheckBox->setChecked(settings.value(QStringLiteral("hasParent"), true).toBool());
	ui->displayLevelComboBox->setCurrentIndex(settings.value(QStringLiteral("level"), QtAutoUpdater::UpdateController::ProgressLevel).toInt());
}

MainWindow::~MainWindow()
{
	QSettings settings{QStringLiteral("./settings.ini"), QSettings::IniFormat};
	settings.setValue(QStringLiteral("path"), ui->configPathLineEdit->text());
	settings.setValue(QStringLiteral("hasParent"), ui->hasParentWindowCheckBox->isChecked());
	settings.setValue(QStringLiteral("level"), ui->displayLevelComboBox->currentIndex());
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
	if (!controller->isRunning()) {
		if (ui->scheduleUpdateDateCheckBox->isChecked()) {
			int id = controller->updater()->scheduleUpdate(ui->scheduleUpdateDateTimeEdit->dateTime());
			if (id)
				qDebug() << "update scheduled with id" << id << "to run at" << ui->scheduleUpdateDateTimeEdit->dateTime();
			else
				qDebug() << "failed to start controller at" << ui->scheduleUpdateDateTimeEdit->dateTime();
		} else
			qDebug() << "start controller:" << controller->start();
	} else
		qDebug() << "start controller:" << false;
}

void MainWindow::on_cancelButton_clicked()
{
	qDebug() << "cancel controller:" << controller->cancelUpdate();
}

void MainWindow::on_activeBox_toggled(bool checked)
{
	if (controller) {
		controller->deleteLater();
		controller = nullptr;
	}

	if (checked) {
		const auto updater = QtAutoUpdater::Updater::create(ui->configPathLineEdit->text());
		if (!updater) {
			QMessageBox::critical(this,
								  tr("Configuration invalid"),
								  tr("The given configuration file either does not exist or is invalid!"));
			ui->activeBox->setChecked(false);
			return;
		}
		if(ui->hasParentWindowCheckBox->isChecked())
			controller = new QtAutoUpdater::UpdateController{updater, this, qApp};
		else
			controller = new QtAutoUpdater::UpdateController{updater, qApp};
		controller->setDesktopFileName(QStringLiteral("WidgetsUpdater"));
		auto action = QtAutoUpdater::UpdateController::createUpdateAction(updater, this);
		action->setIconVisibleInMenu(false);
		ui->menuHelp->addAction(action);
		ui->mainToolBar->addAction(action);

#ifdef Q_OS_OSX
		QMenu *dockMenu = new QMenu{this};
		auto dockAction = controller->createUpdateAction(this);
		dockAction->setMenuRole(QAction::NoRole);
		dockMenu->addAction(dockAction);
		qt_mac_set_dock_menu(dockMenu);
#endif
		connect(controller, &QtAutoUpdater::UpdateController::runningChanged, this, [this](bool running){
			statusBar()->showMessage(running ? tr("running") : tr("not running"));
		});
	} else {
		statusBar()->showMessage(tr("not running"));
	}
	button->setController(controller);
}

void MainWindow::on_hasParentWindowCheckBox_clicked(bool checked)
{
	if(controller)
		controller->setParentWindow(checked ? this : nullptr);
}

void MainWindow::on_displayLevelComboBox_currentIndexChanged(int index)
{
	if (controller)
		controller->setDisplayLevel(static_cast<QtAutoUpdater::UpdateController::DisplayLevel>(index));
}
