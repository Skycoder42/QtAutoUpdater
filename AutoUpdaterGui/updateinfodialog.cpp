#include "updateinfodialog.h"
#include "ui_updateinfodialog.h"
#include <QGuiApplication>
#include <QMessageBox>
#include "messagemaster.h"
using namespace QtAutoUpdater;

UpdateInfoDialog::UpdateInfoDialog(QWidget *parent) :
	QDialog(parent, Qt::WindowCloseButtonHint),
	ui(new Ui::UpdateInfoDialog)
{
	ui->setupUi(this);
	if(!parent)
		this->setWindowModality(Qt::ApplicationModal);
	this->ui->rootLayout->setSpacing(this->ui->rootLayout->contentsMargins().left());

#ifdef Q_OS_OSX
	QFont font = this->ui->headerLabel->font();
	font.setPointSize(20);
	this->ui->headerLabel->setFont(font);
#endif

	QPalette pal(this->ui->headerLabel->palette());
#ifdef Q_OS_WIN32
	// ### hardcoded for now:
	pal.setColor(QPalette::WindowText, QColor(0x00, 0x33, 0x99));
#else
	pal.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::Highlight));
#endif
	this->ui->headerLabel->setPalette(pal);

	this->ui->headerLabel->setText(tr("Updates for %1 are available!")
								   .arg(QGuiApplication::applicationDisplayName()));
	if(QGuiApplication::windowIcon().isNull())
		this->ui->imageLabel->hide();
	else {
		this->ui->imageLabel->show();
		this->ui->imageLabel->setPixmap(QGuiApplication::windowIcon().pixmap(64, 64));//TODO hdpi
	}
}

UpdateInfoDialog::~UpdateInfoDialog()
{
	delete ui;
}

UpdateInfoDialog::DialogResult UpdateInfoDialog::showUpdateInfo(QList<Updater::UpdateInfo> updates, bool &runAsAdmin, bool editable, bool detailed, QWidget *parent)
{
	if(!detailed) {
		QMessageBox mBox(parent);
		mBox.setWindowModality(parent ? Qt::WindowModal : Qt::ApplicationModal);
		mBox.setWindowFlags(mBox.windowFlags() & ~Qt::WindowContextHelpButtonHint);
		mBox.setIcon(QMessageBox::Information);
		mBox.setWindowTitle(tr("Check for Updates"));
		mBox.setWindowIcon(QIcon(QStringLiteral(":/updaterIcons/update.ico")));
		mBox.setText(QStringLiteral("<b>") +
					 tr("Updates for %1 are available!")
					 .arg(QGuiApplication::applicationDisplayName()) +
					 QStringLiteral("</b>"));
		mBox.setInformativeText(tr("There are new updates available! You can install them now or later."));
		QStringList details;
		for(Updater::UpdateInfo info : updates) {
			details << tr("%1 v%2 — %3")
					   .arg(info.name)
					   .arg(info.version.toString())
					   .arg(getByteText(info.size));
		}
		mBox.setDetailedText(details.join(QLatin1Char('\n')));

		QCheckBox *cBox = new QCheckBox(tr("Run with &elevated rights"), &mBox);
		cBox->setEnabled(editable);
		cBox->setChecked(runAsAdmin);
		mBox.setCheckBox(cBox);

		mBox.setDefaultButton(mBox.addButton(tr("Install Now"), QMessageBox::AcceptRole));
		mBox.addButton(tr("Install On Exit"), QMessageBox::ApplyRole);
		mBox.setEscapeButton(mBox.addButton(tr("Install later"), QMessageBox::RejectRole));

		DialogResult res;
		mBox.exec();
		switch (mBox.buttonRole(mBox.clickedButton())) {
		case QMessageBox::AcceptRole:
			res = InstallNow;
			break;
		case QMessageBox::ApplyRole:
			res = InstallLater;
			break;
		case QMessageBox::RejectRole:
			res = NoInstall;
			break;
		default:
			Q_UNREACHABLE();
		}

		if(editable && res != NoInstall)
			runAsAdmin = cBox->isChecked();
		return res;
	} else {
		UpdateInfoDialog dialog(parent);

		for(Updater::UpdateInfo info : updates) {
			QTreeWidgetItem *item = new QTreeWidgetItem(dialog.ui->updateListTreeWidget);
			item->setText(0, info.name);
			item->setText(1, info.version.toString());
			item->setText(2, getByteText(info.size));
			item->setToolTip(2, tr("%L1 Bytes").arg(info.size));
		}
		dialog.ui->updateListTreeWidget->resizeColumnToContents(0);
		dialog.ui->updateListTreeWidget->resizeColumnToContents(1);
		dialog.ui->updateListTreeWidget->resizeColumnToContents(2);

		dialog.ui->runAdminCheckBox->setEnabled(editable);
		dialog.ui->runAdminCheckBox->setChecked(runAsAdmin);

		DialogResult res = (DialogResult)dialog.exec();
		if(editable && res != NoInstall)
			runAsAdmin = dialog.ui->runAdminCheckBox->isChecked();
		return res;
	}
}

void QtAutoUpdater::UpdateInfoDialog::on_acceptButton_clicked()
{
	if(MessageMaster::question(this,
							   tr("Install Now"),
							   tr("Close the application and install updates?"))
		== QMessageBox::Yes) {
		this->accept();
	}
}

void QtAutoUpdater::UpdateInfoDialog::on_delayButton_clicked()
{
	MessageMaster::information(this,
							   tr("Install On Exit"),
							   tr("Updates will be installed on exit. The maintenance tool will be started as soon as you close the application!"));
	this->done(InstallLater);
}

QString UpdateInfoDialog::getByteText(qint64 bytes)
{
	int counter = 0;
	double disNum = bytes;

	while((bytes / 1024) > 0 && counter < 3) {
		disNum = bytes / 1024.;
		bytes = disNum;
		++counter;
	}

	switch(counter) {
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
		return QString();
	}
}
