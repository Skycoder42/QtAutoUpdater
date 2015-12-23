#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QCloseEvent>
using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint),
	ui(new Ui::ProgressDialog)
{
	ui->setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
	delete ui;
}

void ProgressDialog::setCanceled()
{
	this->ui->label->setText(tr("Canceling update check…"));
	this->ui->buttonBox->setEnabled(false);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(this->ui->buttonBox->isEnabled()) {
		this->setCanceled();
		emit canceled();
	}
}
