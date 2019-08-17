#ifndef QTAUTOUPDATER_UPDATEINFODIALOG_H
#define QTAUTOUPDATER_UPDATEINFODIALOG_H

#include <QtAutoUpdaterCore/UpdateInfo>

#include <QtWidgets/QDialog>

#include <qtaskbarcontrol.h>

#include "qtautoupdatergui_global.h"

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

	static DialogResult showUpdateInfo(QList<UpdateInfo> updates,
									   const QString &desktopFileName,
									   bool allowInstallLater,
									   bool canParallelInstall,
									   bool detailed, // TODO allow automatic detection based on if there are 1 or more updates
									   QWidget *parent);

private Q_SLOTS:
	void on_acceptButton_clicked();
	void on_delayButton_clicked();

private:
	QScopedPointer<Ui::UpdateInfoDialog> _ui;
	QTaskbarControl *_taskbar;
	bool _canParallelInstall = false;

	explicit UpdateInfoDialog(QWidget *parent = nullptr);
	~UpdateInfoDialog() override;

	static QString getByteText(quint64 bytes);
	static bool askForClose(QWidget *parent);
	static void informInstallLater(QWidget *parent);
};

}

#endif // QTAUTOUPDATER_UPDATEINFODIALOG_H
