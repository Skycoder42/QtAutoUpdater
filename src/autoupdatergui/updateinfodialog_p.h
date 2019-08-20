#ifndef QTAUTOUPDATER_UPDATEINFODIALOG_H
#define QTAUTOUPDATER_UPDATEINFODIALOG_H

#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtAutoUpdaterCore/UpdaterBackend>

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

	static DialogResult showUpdateInfo(const QList<UpdateInfo> &updates,
									   const QString &desktopFileName,
									   UpdaterBackend::Features features,
									   QWidget *parent);

private Q_SLOTS:
	void on_acceptButton_clicked();
	void on_delayButton_clicked();

private:
	QScopedPointer<Ui::UpdateInfoDialog> _ui;
	QTaskbarControl *_taskbar;
	UpdaterBackend::Features _features;

	explicit UpdateInfoDialog(UpdaterBackend::Features features, QWidget *parent = nullptr);
	~UpdateInfoDialog() override;

	void addUpdates(const QList<UpdateInfo> &updates);

	QString getByteText(quint64 bytes);
};

}

#endif // QTAUTOUPDATER_UPDATEINFODIALOG_H
