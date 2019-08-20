#include "updateinfodialog_p.h"
#include "ui_updateinfodialog.h"
#include "updatecontroller_p.h"

#include <cmath>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <dialogmaster.h>

#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformtheme.h>

using namespace QtAutoUpdater;

UpdateInfoDialog::UpdateInfoDialog(UpdaterBackend::Features features, QWidget *parent) :
	QDialog{parent},
	_ui{new Ui::UpdateInfoDialog},
	_taskbar{new QTaskbarControl{this}},
	_features{features}
{
	_ui->setupUi(this);
	DialogMaster::masterDialog(this);
	_ui->rootLayout->setSpacing(_ui->rootLayout->contentsMargins().left());

#ifdef Q_OS_OSX
	auto font = _ui->headerLabel->font();
	font.setPointSize(20);
	_ui->headerLabel->setFont(font);
#endif

	auto pal = _ui->headerLabel->palette();
#ifdef Q_OS_WIN32
	// ### hardcoded for now:
	pal.setColor(QPalette::WindowText, QColor(0x00, 0x33, 0x99));
#else
	pal.setColor(QPalette::WindowText, QApplication::palette().color(QPalette::Highlight));
#endif
	_ui->headerLabel->setPalette(pal);
	_ui->headerLabel->setText(tr("Updates for %1 are available!")
								   .arg(QApplication::applicationDisplayName()));
	_ui->imageLabel->setPixmap(UpdateControllerPrivate::getUpdatesIcon().pixmap(64, 64));

	//configure buttons and texts
	if (!_features.testFlag(UpdaterBackend::Feature::TriggerInstall)) {
		// no install after exit -> hide install on exit
		_ui->delayButton->setVisible(false);
		if (!_features.testFlag(UpdaterBackend::Feature::PerformInstall)) {
			// can't install at all -> only show a simple "OK"
			_ui->acceptButton->setText(QGuiApplicationPrivate::platformTheme()->standardButtonText(QDialogButtonBox::Ok));
			_ui->skipButton->setVisible(false);
			_ui->stateLabel->setText(tr("There are new updates available! "
										"To install the displayed updates, exit this application and start the updater."));
		} else
			_ui->stateLabel->setText(tr("There are new updates available! You can install them right now by pressing <Install Now> below."));
	} else {
		if (!_features.testFlag(UpdaterBackend::Feature::ParallelInstall)) {
			_ui->stateLabel->setText(tr("There are new updates available! "
										"Installing those requires you to close this application."
										"You can install them now or automatically, when you exit the application."));
		} else {
			_ui->stateLabel->setText(tr("There are new updates available! "
										"You can install them now, without having to exit the application during the update."
										"Alternatively, you can automatically start the installert, when you exit the application."));
		}
	}

	_taskbar->setCounterVisible(false);

	connect(_ui->acceptButton, &QPushButton::clicked,
			this, &UpdateInfoDialog::installNow);
	connect(_ui->delayButton, &QPushButton::clicked,
			this, &UpdateInfoDialog::installLater);
}

UpdateInfoDialog::~UpdateInfoDialog()
{
	_taskbar->setCounterVisible(false);
}

void UpdateInfoDialog::addUpdates(const QList<UpdateInfo> &updates)
{
	for(const auto &info : qAsConst(updates)) {
		auto item = new QTreeWidgetItem{_ui->updateListTreeWidget};
		item->setText(0, info.name());
		item->setToolTip(0, info.name());
		item->setText(1, info.version().toString());
		item->setToolTip(1, info.version().toString());
		item->setText(2, getByteText(info.size()));
		item->setToolTip(2, tr("%L1 Bytes").arg(info.size()));
	}
	_ui->updateListTreeWidget->resizeColumnToContents(0);
	_ui->updateListTreeWidget->resizeColumnToContents(1);
	_ui->updateListTreeWidget->resizeColumnToContents(2);
	_taskbar->setCounter(updates.size());
	_taskbar->setCounterVisible(true);
}

UpdateInfoDialog::DialogResult UpdateInfoDialog::showUpdateInfo(const QList<UpdateInfo> &updates, const QString &desktopFileName, UpdaterBackend::Features features, QWidget *parent)
{
	UpdateInfoDialog dialog{features, parent};
	dialog._taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, desktopFileName);
	dialog.addUpdates(updates);
	return static_cast<DialogResult>(dialog.exec());
}

void QtAutoUpdater::UpdateInfoDialog::installNow()
{
	if (_features.testFlag(UpdaterBackend::Feature::ParallelInstall) ||
		DialogMaster::questionT(this,
								tr("Install Now?"),
								tr("Close the application and install updates?"))
		 == QMessageBox::Yes)
		accept();
}

void QtAutoUpdater::UpdateInfoDialog::installLater()
{
	DialogMaster::informationT(this,
							   tr("Install On Exit"),
							   tr("Updates will be installed on exit. The update tool "
								  "will be started as soon as you close the application!"));
	done(InstallLater);
}

QString UpdateInfoDialog::getByteText(quint64 bytes)
{
	auto counter = 0;
	auto disNum = static_cast<double>(bytes);

	while ((bytes / 1024ull) > 0 && counter < 3) {
		disNum = bytes / 1024.;
		bytes = static_cast<quint64>(std::floor(disNum));
		++counter;
	}

	switch (counter) {
	case 0:
		return tr("%L1 Bytes").arg(bytes);
	case 1:
		return tr("%L1 KiB").arg(disNum, 0, 'f', 2);
	case 2:
		return tr("%L1 MiB").arg(disNum, 0, 'f', 2);
	case 3:
		return tr("%L1 GiB").arg(disNum, 0, 'f', 2);
	default:
		Q_UNREACHABLE();
		return {};
	}
}
