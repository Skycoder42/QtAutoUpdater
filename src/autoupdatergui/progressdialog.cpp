#include "progressdialog_p.h"
#include "ui_progressdialog.h"

#include <QtGui/QCloseEvent>
#include <dialogmaster.h>

using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog{parent},
	ui{new Ui::ProgressDialog}
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this, true, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
}

ProgressDialog::~ProgressDialog() = default;

void ProgressDialog::setCanceled()
{
	ui->label->setText(tr("Canceling update check…"));
	ui->buttonBox->setEnabled(false);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(ui->buttonBox->isEnabled()) {
		setCanceled();
		emit canceled();
	}
}

