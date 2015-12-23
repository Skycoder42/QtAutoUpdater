#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QCloseEvent>

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

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(this->ui->buttonBox->isEnabled()) {
		this->ui->label->setText(tr("Canceling update check…"));
		this->ui->buttonBox->setEnabled(false);
		emit canceled();
	}
}
