#ifndef QTAUTOUPDATER_UPDATEINFODIALOG_H
#define QTAUTOUPDATER_UPDATEINFODIALOG_H

#include <tuple>

#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtWidgets/QDialog>

#include <qtaskbarcontrol.h>

#include "qtautoupdaterwidgets_global.h"

namespace Ui {
class UpdateInfoDialog;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT UpdateInfoDialog : public QDialog
{
	Q_OBJECT

public:
	enum DialogResult {
		InstallNow = QDialog::Accepted,
		InstallLater = QDialog::Accepted + 1,
		NoInstall = QDialog::Rejected
	};
	Q_ENUM(DialogResult)

	static DialogResult showSimpleInfo(const QList<UpdateInfo> &updates,
									   UpdaterBackend::Features features,
									   QWidget *parent);
	static DialogResult showUpdateInfo(const QList<UpdateInfo> &updates,
									   const QString &desktopFileName,
									   UpdaterBackend::Features features,
									   UpdaterBackend::SecondaryInfo secInfo,
									   QWidget *parent);

private Q_SLOTS:
	void installNow();
	void installLater();

private:
	QScopedPointer<Ui::UpdateInfoDialog> _ui;
	QTaskbarControl *_taskbar;
	UpdaterBackend::Features _features;
	UpdaterBackend::SecondaryInfo _secInfo;

	explicit UpdateInfoDialog(UpdaterBackend::Features features, UpdaterBackend::SecondaryInfo &&secInfo, QWidget *parent = nullptr);
	~UpdateInfoDialog() override;

	void addUpdates(const QList<UpdateInfo> &updates);

	static std::tuple<QString, bool, bool> capabilities(UpdaterBackend::Features features); // (text, install, onExit)
};

}

#endif // QTAUTOUPDATER_UPDATEINFODIALOG_H
