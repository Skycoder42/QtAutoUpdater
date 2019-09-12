#ifndef QTAUTOUPDATER_PROCESSBACKEND_P_H
#define QTAUTOUPDATER_PROCESSBACKEND_P_H

#include <QtCore/qglobal.h>

#if QT_CONFIG(process)

#include "processbackend.h"
#include "updaterbackend_p.h"

#include <QtCore/QHash>
#include <QtCore/QProcess>

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT ProcessBackendPrivate : public UpdaterBackendPrivate
{
	Q_DECLARE_PUBLIC(ProcessBackend)

public:
	using ProcessInfoBase = ProcessBackend::ProcessInfoBase;
	using UpdateProcessInfo = ProcessBackend::UpdateProcessInfo;
	using InstallProcessInfo = ProcessBackend::InstallProcessInfo;

	ProcessBackendPrivate(QString &&pKey);

	QHash<int, std::pair<UpdateProcessInfo, QProcess*>> updateProcesses;
	QPointer<QProcess> installProc;

	void _q_updaterStateChanged(int id, QProcess::ProcessState state);
	void _q_installerStateChanged(QProcess::ProcessState state);

	QProcess *createProc(const ProcessInfoBase &info);
};

Q_DECLARE_LOGGING_CATEGORY(logProcessBackend)

}

#endif

#endif // QTAUTOUPDATER_PROCESSBACKEND_P_H
