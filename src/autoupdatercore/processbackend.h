#ifndef QTAUTOUPDATER_PROCESSBACKEND_H
#define QTAUTOUPDATER_PROCESSBACKEND_H

#include <QtCore/qglobal.h>

#if QT_CONFIG(process)

#include <optional>

#include <QtCore/QProcess>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updaterbackend.h"

namespace QtAutoUpdater {

class ProcessBackendPrivate;
class Q_AUTOUPDATERCORE_EXPORT ProcessBackend : public UpdaterBackend
{
	Q_OBJECT

public:
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

protected:
	struct Q_AUTOUPDATERCORE_EXPORT ProcessInfoBase {
		QString program;
		QStringList arguments;
		std::optional<QString> workingDir = std::nullopt;
	};

	struct Q_AUTOUPDATERCORE_EXPORT UpdateProcessInfo : public ProcessInfoBase {
		bool useStdout = true;
		bool useStderr = false;
		std::optional<QByteArray> stdinData;
	};

	struct Q_AUTOUPDATERCORE_EXPORT InstallProcessInfo : public ProcessInfoBase {
		std::optional<bool> runAsAdmin = std::nullopt;
	};

	explicit ProcessBackend(QString &&key, QObject *parent = nullptr);
	explicit ProcessBackend(ProcessBackendPrivate &dd, QObject *parent = nullptr);

	void runUpdateTool(int id, UpdateProcessInfo toolInfo);
	virtual void onToolDone(int id, int exitCode, QIODevice *processDevice) = 0;
	virtual std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) = 0;

private:
	Q_DECLARE_PRIVATE(ProcessBackend)

	Q_PRIVATE_SLOT(d_func(), void _q_updaterStateChanged(int, QProcess::ProcessState state))
	Q_PRIVATE_SLOT(d_func(), void _q_installerStateChanged(QProcess::ProcessState state))
};

}

#endif

#endif // QTAUTOUPDATER_PROCESSBACKEND_H
