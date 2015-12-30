#ifndef UPDATEINFODIALOG_H
#define UPDATEINFODIALOG_H

#include <QDialog>
#include <updater.h>

namespace Ui {
	class UpdateInfoDialog;
}

namespace QtAutoUpdater
{
	class UpdateInfoDialog : public QDialog
	{
		Q_OBJECT

	public:
		enum DialogResult {
			InstallNow = QDialog::Accepted,
			InstallLater = QDialog::Accepted + 1,
			NoInstall = QDialog::Rejected
		};

		explicit UpdateInfoDialog(QWidget *parent = 0);
		~UpdateInfoDialog();

		void setNewParent(QWidget *parent);

		//TODO make static
		DialogResult showUpdateInfo(QList<Updater::UpdateInfo> updates, bool &runAsAdmin, bool editable);

	private slots:
		void on_acceptButton_clicked();
		void on_delayButton_clicked();

	private:
		Ui::UpdateInfoDialog *ui;

		static QString getByteText(qint64 bytes);
	};
}

#endif // UPDATEINFODIALOG_H
