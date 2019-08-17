#include "updateinfodialog_p.h"
#include "ui_updateinfodialog.h"
#include "updatecontroller_p.h"

#include <cmath>

#include <QtWidgets/QApplication>
#include <dialogmaster.h>

using namespace QtAutoUpdater;

UpdateInfoDialog::UpdateInfoDialog(QWidget *parent) :
	QDialog{parent},
	_ui{new Ui::UpdateInfoDialog},
	_taskbar{new QTaskbarControl{this}}
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
}

UpdateInfoDialog::~UpdateInfoDialog()
{
	_taskbar->setCounterVisible(false);
}

UpdateInfoDialog::DialogResult UpdateInfoDialog::showUpdateInfo(QList<UpdateInfo> updates, const QString &desktopFileName, bool allowInstallLater, bool canParallelInstall, bool detailed, QWidget *parent)
{
	if (!detailed) {
		DialogMaster::MessageBoxInfo boxInfo;
		boxInfo.parent = parent;
		boxInfo.windowTitle = tr("Check for Updates");
		boxInfo.icon = UpdateControllerPrivate::getUpdatesIcon();
		boxInfo.title = tr("Updates for %1 are available!")
						.arg(QApplication::applicationDisplayName());
		boxInfo.text = tr("There are new updates available! You can install them now or later.");
		QStringList details;
		for (const auto &info : qAsConst(updates)) {
			details << tr("%1 v%2 — %3")
					   .arg(info.name(), info.version().toString(), getByteText(info.size()));
		}
		boxInfo.details = details.join(QLatin1Char('\n'));

		boxInfo.buttonTexts.insert(QMessageBox::Ok, tr("Install Now"));
		if (allowInstallLater)
			boxInfo.buttonTexts.insert(QMessageBox::Apply, tr("Install On Exit"));
		boxInfo.buttonTexts.insert(QMessageBox::Cancel, tr("Install later"));

		QScopedPointer<QMessageBox> box{DialogMaster::createMessageBox(boxInfo)};

		switch (box->exec()) {
		case QMessageBox::Ok:
			if (canParallelInstall || askForClose(parent))
				return InstallNow;
			else
				return NoInstall;
		case QMessageBox::Apply:
			informInstallLater(parent);
			return InstallLater;
		case QMessageBox::Cancel:
			return NoInstall;
		default:
			Q_UNREACHABLE();
		}
	} else {
		UpdateInfoDialog dialog{parent};

		for(const auto &info : qAsConst(updates)) {
			auto item = new QTreeWidgetItem{dialog._ui->updateListTreeWidget};
			item->setText(0, info.name());
			item->setToolTip(0, info.name());
			item->setText(1, info.version().toString());
			item->setToolTip(1, info.version().toString());
			item->setText(2, getByteText(info.size()));
			item->setToolTip(2, tr("%L1 Bytes").arg(info.size()));
		}
		dialog._ui->updateListTreeWidget->resizeColumnToContents(0);
		dialog._ui->updateListTreeWidget->resizeColumnToContents(1);
		dialog._ui->updateListTreeWidget->resizeColumnToContents(2);
		dialog._ui->delayButton->setVisible(allowInstallLater);
		dialog._taskbar->setAttribute(QTaskbarControl::LinuxDesktopFile, desktopFileName);
		dialog._taskbar->setCounter(updates.size());
		dialog._taskbar->setCounterVisible(true);
		dialog._canParallelInstall = canParallelInstall;

		return static_cast<DialogResult>(dialog.exec());
	}
}

void QtAutoUpdater::UpdateInfoDialog::on_acceptButton_clicked()
{
	if (_canParallelInstall || askForClose(this))
		accept();
}

void QtAutoUpdater::UpdateInfoDialog::on_delayButton_clicked()
{
	informInstallLater(this);
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

bool UpdateInfoDialog::askForClose(QWidget *parent)
{
	return DialogMaster::questionT(parent,
								   tr("Install Now?"),
								   tr("Close the application and install updates?"))
			== QMessageBox::Yes;
}

void UpdateInfoDialog::informInstallLater(QWidget *parent)
{
	DialogMaster::informationT(parent,
							   tr("Install On Exit"),
							   tr("Updates will be installed on exit. The update tool "
								  "will be started as soon as you close the application!"));
}
