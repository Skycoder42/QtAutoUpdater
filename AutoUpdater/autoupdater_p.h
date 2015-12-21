#ifndef AUTOUPDATER_P_H
#define AUTOUPDATER_P_H

#include "autoupdater.h"
#include <QTimer>
#include <QProcess>

class AutoUpdaterPrivate
{
private:
	struct RunInfo
	{
		QString toolPath;
		QStringList runArgs;
		bool runAdmin;
	};

	AutoUpdater *q_ptr;
	Q_DECLARE_PUBLIC(AutoUpdater)

	bool running;
	RunInfo mainInfo;

	QList<AutoUpdater::UpdateInfo> infos;
	int lastErrorCode;
	QString lastErrorLog;

	RunInfo workingInfo;
	QTimer *waitTimer;
	QProcess *mainProcess;

	inline AutoUpdaterPrivate(AutoUpdater *q_ptr);

	static const QString toSystemExe(const QString basePath);

	AutoUpdaterPrivate *createWorkingCopy() const;
//	static int runUpdateChecker(AutoUpdaterPrivate *workingCopy, int delay);
};

#endif // AUTOUPDATER_P_H
