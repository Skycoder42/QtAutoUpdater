#include "progressdialog_p.h"
#include "ui_progressdialog.h"

#include <QtGui/QCloseEvent>
#include <dialogmaster.h>

using namespace QtAutoUpdater;

ProgressDialog::ProgressDialog(const QString &desktopFileName, QWidget *parent) :
	QDialog{parent},
	_ui{new Ui::ProgressDialog},
	_taskbar{new QTaskbarControl{this}}
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this, true, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	_taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, desktopFileName);
}

ProgressDialog::~ProgressDialog() = default;

void ProgressDialog::open(Updater *pUpdater)
{
	_updater = pUpdater;
	connect(_updater, &Updater::progressChanged,
			this, &ProgressDialog::updateProgress);
	connect(this, &ProgressDialog::canceled,
			_updater, [this]() {_updater->abortUpdateCheck(); });
	QDialog::open();
	_taskbar->setProgress(-1.0);
	_taskbar->setProgressVisible(true);
}

void ProgressDialog::setCanceled()
{
	_ui->label->setText(tr("Canceling update check…"));
	_ui->buttonBox->setEnabled(false);
	_taskbar->setAttribute(QTaskbarControl::WindowsProgressState, QTaskbarControl::Paused);
}

void ProgressDialog::updateProgress(double progress, const QString &status)
{
	_taskbar->setProgress(progress);
	if (progress < 0) {
		if (_ui->progressBar->maximum() != 0) {
			_ui->progressBar->setValue(0);
			_ui->progressBar->setRange(0, 0);
			_ui->progressBar->setTextVisible(false);
		}
	} else {
		if (_ui->progressBar->maximum() == 0) {
			_ui->progressBar->setRange(0, 1000);
			_ui->progressBar->setValue(0);
			_ui->progressBar->setTextVisible(true);
		}
		_ui->progressBar->setValue(static_cast<int>(progress * 1000));
	}

	if (_updater->state() == Updater::State::Checking &&
		!status.isEmpty())
		_ui->label->setText(status);
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(_ui->buttonBox->isEnabled()) {
		setCanceled();
		emit canceled();
	}
}
