#include "updateinfodialog.h"
#include "ui_updateinfodialog.h"
#include <QGuiApplication>
#include <QMessageBox>
using namespace QtAutoUpdater;

UpdateInfoDialog::UpdateInfoDialog(QWidget *parent) :
	QDialog(parent, Qt::WindowCloseButtonHint),
	ui(new Ui::UpdateInfoDialog)
{
	ui->setupUi(this);
	this->ui->rootLayout->setSpacing(this->ui->rootLayout->contentsMargins().left());

	QPalette pal(this->ui->headerLabel->palette());
#ifdef Q_OS_WIN32
	// ### hardcoded for now:
	pal.setColor(QPalette::WindowText, QColor(0x00, 0x33, 0x99));
#else
	pal.setColor(QPalette::WindowText, QGuiApplication::palette().color(QPalette::Highlight));
#endif
	this->ui->headerLabel->setPalette(pal);
}

UpdateInfoDialog::~UpdateInfoDialog()
{
	delete ui;
}

UpdateInfoDialog::DialogResult UpdateInfoDialog::showUpdateInfo(QList<Updater::UpdateInfo> updates)
{
	this->ui->headerLabel->setText(tr("Updates for %1 are available!")
								   .arg(QGuiApplication::applicationDisplayName()));
	if(QGuiApplication::windowIcon().isNull())
		this->ui->imageLabel->hide();
	else {
		this->ui->imageLabel->show();
		this->ui->imageLabel->setPixmap(QGuiApplication::windowIcon().pixmap(64, 64));
	}

	this->ui->updateListTreeWidget->clear();
	for(Updater::UpdateInfo info : updates) {
		QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->updateListTreeWidget);
		item->setText(0, info.name);
		item->setText(1, info.version.toString());
		item->setText(2, getByteText(info.size));
	}
	this->ui->updateListTreeWidget->resizeColumnToContents(0);
	this->ui->updateListTreeWidget->resizeColumnToContents(1);
	this->ui->updateListTreeWidget->resizeColumnToContents(2);

	return (UpdateInfoDialog::DialogResult)this->exec();
}

void QtAutoUpdater::UpdateInfoDialog::on_delayButton_clicked()
{
	if(QMessageBox::question(this,
							 tr("Install later"),
							 tr("If you choose to install the updates on exit, the maintenance tool will be started as soon as you close the application!"),
							 QMessageBox::Ok,
							 QMessageBox::Cancel)
		== QMessageBox::Ok) {
		this->done(InstallLater);
	}
}

QString UpdateInfoDialog::getByteText(qint64 bytes)
{
	int counter = 0;
	double disNum = bytes;

	while((bytes / 1024) > 0 && counter < 3) {
		disNum = bytes / 1024.;
		qDebug() << disNum << bytes;
		bytes = disNum;
		++counter;
	}

	switch(counter) {
	case 0:
		return tr("%L1 Bytes").arg(bytes);
	case 1:
		return tr("%L1 KB").arg(disNum, 0, 'f', 2);
	case 2:
		return tr("%L1 MB").arg(disNum, 0, 'f', 2);
	case 3:
		return tr("%L1 GB").arg(disNum, 0, 'f', 2);
	default:
		Q_UNREACHABLE();
		return QString();
	}
}
