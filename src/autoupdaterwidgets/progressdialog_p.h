#ifndef QTAUTOUPDATER_PROGRESSDIALOG_P_H
#define QTAUTOUPDATER_PROGRESSDIALOG_P_H

#include <functional>

#include <QtCore/QPointer>
#include <QtCore/QScopedPointer>

#include <QtAutoUpdaterCore/Updater>

#include <QtWidgets/QDialog>
#include <QtWidgets/QMessageBox>

#include <qtaskbarcontrol.h>

#include "qtautoupdaterwidgets_global.h"

namespace Ui {
class ProgressDialog;
}

namespace QtAutoUpdater
{

class Q_AUTOUPDATERWIDGETS_EXPORT ProgressDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProgressDialog(const QString &desktopFileName, QWidget *parent = nullptr);
	~ProgressDialog() override;

	void open(Updater *pUpdater);
	void setCanceled();

public Q_SLOTS:
	void accept() override {}
	void reject() override {}

Q_SIGNALS:
	void canceled();

private Q_SLOTS:
	void updateProgress(double progress, const QString &status);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	QScopedPointer<Ui::ProgressDialog> _ui;
	QPointer<Updater> _updater;
	QTaskbarControl *_taskbar;
};

}

#endif // QTAUTOUPDATER_PROGRESSDIALOG_P_H
