#include "updateinfodialog_p.h"
#include "ui_updateinfodialog.h"
#include "updatecontroller_p.h"

#include <cmath>

#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <dialogmaster.h>

#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformtheme.h>

#include <QtAutoUpdaterCore/private/updater_p.h>

using namespace QtAutoUpdater;

UpdateInfoDialog::DialogResult UpdateInfoDialog::showSimpleInfo(const QList<UpdateInfo> &updates, UpdaterBackend::Features features, QWidget *parent)
{
	DialogMaster::MessageBoxInfo mboxInfo;
	mboxInfo.parent = parent;
	mboxInfo.windowTitle = tr("Check for Updates");
	mboxInfo.icon = UpdateControllerPrivate::getUpdatesIcon();
	mboxInfo.title = tr("Updates for %1 are available!")
					 .arg(QApplication::applicationDisplayName());
	mboxInfo.buttons = QMessageBox::NoButton;

	auto [text, canInstall, canOnExit] = capabilities(features);
	mboxInfo.text = std::move(text);
	if (canOnExit)
		mboxInfo.buttonTexts.insert(QMessageBox::Apply, tr("Install on exit"));
	if (canInstall) {
		mboxInfo.buttonTexts.insert(QMessageBox::Ok, tr("Install now"));
		mboxInfo.buttonTexts.insert(QMessageBox::Cancel, tr("Install later"));
		mboxInfo.escapeButton = QMessageBox::Cancel;
	} else {
		mboxInfo.buttons = QMessageBox::Ok;
		mboxInfo.escapeButton = QMessageBox::Ok;
	}
	mboxInfo.defaultButton = QMessageBox::Ok;

	QStringList updateDetails;
	updateDetails.reserve(updates.size());
	for (const auto &info : updates)
		updateDetails.append(tr("%1 – %2").arg(info.name(), info.version().toString()));
	mboxInfo.details = updateDetails.join(QLatin1Char('\n'));

	switch (DialogMaster::messageBox(mboxInfo)) {
	case QMessageBox::Ok:
		return InstallNow;
	case QMessageBox::Apply:
		return InstallLater;
	case QMessageBox::Cancel:
		return NoInstall;
	default:
		Q_UNREACHABLE();
		return NoInstall;
	}
}

UpdateInfoDialog::DialogResult UpdateInfoDialog::showUpdateInfo(const QList<UpdateInfo> &updates, const QString &desktopFileName, UpdaterBackend::Features features, UpdaterBackend::SecondaryInfo secInfo, QWidget *parent)
{
	UpdateInfoDialog dialog{features, std::move(secInfo), parent};
	dialog._taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, desktopFileName);
	dialog.addUpdates(updates);
	return static_cast<DialogResult>(dialog.exec());
}

UpdateInfoDialog::UpdateInfoDialog(UpdaterBackend::Features features, UpdaterBackend::SecondaryInfo &&secInfo, QWidget *parent) :
	QDialog{parent},
	_ui{new Ui::UpdateInfoDialog},
	_taskbar{new QTaskbarControl{this}},
	_features{features},
	_secInfo{std::move(secInfo)}
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

	if (_secInfo)
		_ui->updateListTreeWidget->headerItem()->setText(2, _secInfo->second);

	connect(_ui->acceptButton, &QPushButton::clicked,
			this, &UpdateInfoDialog::installNow);
	connect(_ui->delayButton, &QPushButton::clicked,
			this, &UpdateInfoDialog::installLater);

	//configure buttons and texts
	auto [text, canInstall, canOnExit] = capabilities(_features);
	_ui->stateLabel->setText(text);
	if (!canOnExit)
		_ui->delayButton->setVisible(false);
	if (!canInstall) {
		_ui->acceptButton->setText(QGuiApplicationPrivate::platformTheme()->standardButtonText(QDialogButtonBox::Ok));
		_ui->skipButton->setVisible(false);
		disconnect(_ui->acceptButton, &QPushButton::clicked,
				   this, &UpdateInfoDialog::installNow);
		connect(_ui->acceptButton, &QPushButton::clicked,
				this, &UpdateInfoDialog::reject);
	}

	_taskbar->setCounterVisible(false);
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
		if (_secInfo) {
			qDebug() << _secInfo->first << info.data().value(_secInfo->first);
			item->setText(2, info.data().value(_secInfo->first).toString());
		}
	}
	_ui->updateListTreeWidget->resizeColumnToContents(0);
	_ui->updateListTreeWidget->resizeColumnToContents(1);
	if (_secInfo)
		_ui->updateListTreeWidget->resizeColumnToContents(2);
	_taskbar->setCounter(updates.size());
	_taskbar->setCounterVisible(true);
}

void QtAutoUpdater::UpdateInfoDialog::installNow()
{
	if (_features.testFlag(UpdaterBackend::Feature::ParallelTrigger) ||
		_features.testFlag(UpdaterBackend::Feature::PerformInstall) ||
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

std::tuple<QString, bool, bool> UpdateInfoDialog::capabilities(UpdaterBackend::Features features)
{
	if (!features.testFlag(UpdaterBackend::Feature::TriggerInstall)) {
		if (!features.testFlag(UpdaterBackend::Feature::PerformInstall)) {
			// can't install at all -> only show a simple "OK"
			return {
				tr("There are new updates available! "
				   "To install the displayed updates, exit this application and start the updater."),
				false,
				false
			};
		} else {
			// no install after exit -> only show normal install
			return {
				tr("There are new updates available! You can install them right now by pressing <Install Now> below."),
				true,
				false
			};
		}
	} else {
		if (features.testFlag(UpdaterBackend::Feature::ParallelTrigger) ||
			features.testFlag(UpdaterBackend::Feature::PerformInstall)) {
			return {
				tr("There are new updates available! "
				   "You can install them now, without having to exit the application during the update. "
				   "Alternatively, you can automatically start the installer, when you exit the application."),
				true,
				true
			};
		} else {
			return {
				tr("There are new updates available! "
				   "Installing those requires you to close this application. "
				   "You can install them now or automatically, when you exit the application."),
				true,
				true
			};
		}
	}
}
