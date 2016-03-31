#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QCloseEvent>
#include <dialogmaster.h>
#ifdef Q_OS_WIN
#include <QWinTaskbarProgress>
#endif
#include <QDebug>
using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProgressDialog)
#ifdef Q_OS_WIN
	,tButton(new QWinTaskbarButton(this))
#endif
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this, true, Qt::CustomizeWindowHint);//TODO 2 methods
	Qt::WindowFlags flags = this->windowFlags();
	flags &= ~Qt::WindowSystemMenuHint;
	flags &= ~Qt::WindowCloseButtonHint;
	this->setWindowFlags(flags);
#ifdef Q_OS_WIN
	if(parent)
		this->setupTaskbar(parent);
#endif
}

ProgressDialog::~ProgressDialog()
{
#ifdef Q_OS_WIN
	this->tButton->progress()->hide();
#endif
	delete ui;
}

void ProgressDialog::setCanceled()
{
	this->ui->label->setText(tr("Canceling update check…"));
	this->ui->buttonBox->setEnabled(false);
}

void ProgressDialog::hide(QMessageBox::Icon hideType)
{
#ifdef Q_OS_WIN
	if(this->tButton->window()) {
		QWinTaskbarProgress *progress = this->tButton->progress();
		progress->setRange(0, 1);
		progress->setValue(1);
		switch (hideType) {
		case QMessageBox::Information:
			progress->resume();
			break;
		case QMessageBox::Warning:
			progress->pause();
			break;
		case QMessageBox::Critical:
			progress->stop();
			break;
		default:
			progress->hide();
			break;
		}
	}
#else
	Q_UNUSED(hideType)
#endif
	this->QDialog::hide();
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(this->ui->buttonBox->isEnabled()) {
		this->setCanceled();
		emit canceled();
	}
}

#ifdef Q_OS_WIN
void ProgressDialog::showEvent(QShowEvent *event)
{
	event->accept();
	this->setupTaskbar(this);
}

void ProgressDialog::setupTaskbar(QWidget *window)
{
	if(!this->tButton->window()) {
		this->tButton->setWindow(window->windowHandle());
		QWinTaskbarProgress *progress = this->tButton->progress();
		progress->setRange(0, 0);
		progress->resume();
		progress->show();
	}
}
#endif

