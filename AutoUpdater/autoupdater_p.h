#ifndef AUTOUPDATER_P_H
#define AUTOUPDATER_P_H

#include "autoupdater.h"
#include <QTimer>
#include <QProcess>

template<typename... Args> struct SELECT {
	template<typename C, typename R>
	static Q_DECL_CONSTEXPR auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
		return pmf;
	}
};

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

	RunInfo mainInfo;
	QList<AutoUpdater::UpdateInfo> updateInfos;
	bool normalExit;
	int lastErrorCode;
	QByteArray lastErrorLog;

	bool running;
	RunInfo workingInfo;
	QProcess *mainProcess;

	inline AutoUpdaterPrivate(AutoUpdater *q_ptr);

	static const QString toSystemExe(const QString basePath);

	void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
	void updaterError(QProcess::ProcessError error);
};

#endif // AUTOUPDATER_P_H
