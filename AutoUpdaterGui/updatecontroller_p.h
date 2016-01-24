#ifndef UPDATECONTROLLER_P_H
#define UPDATECONTROLLER_P_H

#include <QtGlobal>
#ifdef Q_OS_WIN
#include <QPointer>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif
#include <updater.h>
#include "updatecontroller.h"
#include "progressdialog.h"
#include "updateinfodialog.h"

namespace QtAutoUpdater
{
	class UpdateControllerPrivate
	{
	public:
		UpdateController *q_ptr;
		Q_DECLARE_PUBLIC(UpdateController)

		QWidget *window;

		UpdateController::DisplayLevel displayLevel;
		bool running;
		Updater *mainUpdater;
		bool runAdmin;
		bool adminUserEdit;
		QStringList runArgs;

		ProgressDialog *checkUpdatesProgress;
		bool wasCanceled;
		UpdateInfoDialog *infoDialog;
#ifdef Q_OS_WIN
		QWinTaskbarButton *tButton;
		QPointer<QWinTaskbarProgress> tProgress;
#endif

		QHash<int, UpdateController::DisplayLevel> updateTasks;

		UpdateControllerPrivate(UpdateController *q_ptr, QWidget *window);
		UpdateControllerPrivate(UpdateController *q_ptr, const QString &toolPath, QWidget *window);
		~UpdateControllerPrivate();
	};

#ifdef Q_OS_WIN
	class ShowEventFilter : public QObject
	{
	public:
		ShowEventFilter(UpdateControllerPrivate *controller);
		bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
	private:
		UpdateControllerPrivate *controller;
	};
#endif
}

#endif // UPDATECONTROLLER_P_H
