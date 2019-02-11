#include "progressdialog_p.h"
#include "ui_progressdialog.h"

#include <QtGui/QCloseEvent>
#include <dialogmaster.h>

using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog{parent},
	ui{new Ui::ProgressDialog},
	taskbar{new QTaskbarControl{this}}
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this, true, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

	taskbar->setProgress(-1.0);
	taskbar->setProgressVisible(true);
}

ProgressDialog::~ProgressDialog() = default;

void ProgressDialog::setCanceled()
{
	ui->label->setText(tr("Canceling update check…"));
	ui->buttonBox->setEnabled(false);
}

void ProgressDialog::hide(QMessageBox::Icon hideType)
{
	taskbar->setProgress(1.0);
	switch (hideType) {
	case QMessageBox::Information:
		taskbar->setAttribute(QTaskbarControl::WindowsProgressState, QTaskbarControl::Running);
		taskbar->setProgressVisible(true);
		break;
	case QMessageBox::Warning:
		taskbar->setAttribute(QTaskbarControl::WindowsProgressState, QTaskbarControl::Paused);
		taskbar->setProgressVisible(true);
		break;
	case QMessageBox::Critical:
		taskbar->setAttribute(QTaskbarControl::WindowsProgressState, QTaskbarControl::Stopped);
		taskbar->setProgressVisible(true);
		break;
	default:
		taskbar->setProgressVisible(false);
		break;
	}

	QDialog::hide();
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(ui->buttonBox->isEnabled()) {
		setCanceled();
		emit canceled();
	}
}

