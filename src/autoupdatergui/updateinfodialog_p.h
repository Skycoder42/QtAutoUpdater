#ifndef QTAUTOUPDATER_UPDATEINFODIALOG_H
#define QTAUTOUPDATER_UPDATEINFODIALOG_H

#include "qtautoupdatergui_global.h"

#include <QtAutoUpdaterCore/updater.h>

#include <QtWidgets/QDialog>

namespace Ui {
class UpdateInfoDialog;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERGUI_EXPORT UpdateInfoDialog : public QDialog
{
	Q_OBJECT

public:
	enum DialogResult {
		InstallNow = QDialog::Accepted,
		InstallLater = QDialog::Accepted + 1,
		NoInstall = QDialog::Rejected
	};
	Q_ENUM(DialogResult)

	static DialogResult showUpdateInfo(QList<Updater::UpdateInfo> updates,
									   bool &runAsAdmin,
									   bool editable,
									   bool detailed,
									   QWidget *parent);

private Q_SLOTS:
	void on_acceptButton_clicked();
	void on_delayButton_clicked();

private:
	QScopedPointer<Ui::UpdateInfoDialog> ui;

	explicit UpdateInfoDialog(QWidget *parent = nullptr);
	~UpdateInfoDialog();

	static QString getByteText(qint64 bytes);
};

}

#endif // QTAUTOUPDATER_UPDATEINFODIALOG_H
