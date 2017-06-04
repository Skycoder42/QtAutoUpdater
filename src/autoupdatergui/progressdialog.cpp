#include "progressdialog_p.h"
#include "ui_progressdialog.h"

#include <QtGui/QCloseEvent>
#include <dialogmaster.h>
#ifdef Q_OS_WIN
#include <QtWinExtras/QWinTaskbarProgress>
#endif

using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProgressDialog)
#ifdef Q_OS_WIN
	,tButton(new QWinTaskbarButton(this))
#endif
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this, true, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
#ifdef Q_OS_WIN
	if(parent)
		setupTaskbar(parent);
#endif
}

ProgressDialog::~ProgressDialog()
{
#ifdef Q_OS_WIN
	tButton->progress()->hide();
#endif
}

void ProgressDialog::setCanceled()
{
	ui->label->setText(tr("Canceling update check…"));
	ui->buttonBox->setEnabled(false);
}

void ProgressDialog::hide(QMessageBox::Icon hideType)
{
#ifdef Q_OS_WIN
	if(tButton->window()) {
		QWinTaskbarProgress *progress = tButton->progress();
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

#ifdef Q_OS_WIN
void ProgressDialog::showEvent(QShowEvent *event)
{
	event->accept();
	setupTaskbar(this);
}

void ProgressDialog::setupTaskbar(QWidget *window)
{
	if(!tButton->window()) {
		tButton->setWindow(window->windowHandle());
		QWinTaskbarProgress *progress = tButton->progress();
		progress->setRange(0, 0);
		progress->resume();
		progress->show();
	}
}
#endif

