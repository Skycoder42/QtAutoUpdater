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
//! An extension of UpdaterBackend for easy implementation of QProcess based updater plugins
class Q_AUTOUPDATERCORE_EXPORT ProcessBackend : public UpdaterBackend
{
	Q_OBJECT

public:
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

	//! Helper function to convert a variant value into a string list of paths
	static QStringList readPathList(const QVariant &value);
	//! Helper function to convert a variant value into a string list of process arguments
	static QStringList readArgumentList(const QVariant &value);

protected:
	//! Structure that collects information about a process to be started
	struct Q_AUTOUPDATERCORE_EXPORT ProcessInfoBase {
		QString program;  //!< The path or name of the executable
		QStringList arguments;  //!< The arguments that should be passed to the process
		std::optional<QString> workingDir = std::nullopt;  //!< The working directy. If set to nullopt, the programs path is used
	};

	//! Structure that collects information about an update process to be started
	struct Q_AUTOUPDATERCORE_EXPORT UpdateProcessInfo : public ProcessInfoBase {
		bool useStdout = true; //!< Keep the stdout of the running process so it can be processed. Otherwise it is discarded
		bool useStderr = false; //!< Keep the stderr of the running process so it can be processed. Otherwise it is forwarded to the current processes stderr
		std::optional<QByteArray> stdinData; //!< Data to be passed to the stdin of the process once it started. Set this to nullopt to disable input
	};

	//! Structure that collects information about an installation process to be started
	struct Q_AUTOUPDATERCORE_EXPORT InstallProcessInfo : public ProcessInfoBase {
		std::optional<bool> runAsAdmin = std::nullopt; //!< Specify, if the process should be run as admin. If set to nullopt, the backend tries to figure this out by using AdminAuthoriser::needsAdminPermission(const QString &)
	};

	//! Constructor using the backends key and a parent
	explicit ProcessBackend(QString &&key, QObject *parent = nullptr);
	//! @private
	explicit ProcessBackend(ProcessBackendPrivate &dd, QObject *parent = nullptr);

	//! Starts the given updater tool for the id
	void runUpdateTool(int id, UpdateProcessInfo toolInfo);
	//! Send a termination request to the given process
	void cancelUpdateTool(int id, bool kill = false);
	//! Is called by the backend when the updater tool started for id has finished with a result
	virtual void onToolDone(int id, int exitCode, QIODevice *processDevice) = 0;
	//! Is called to get information about the tool to be run as external installer
	virtual std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) = 0;

private:
	Q_DECLARE_PRIVATE(ProcessBackend)

	Q_PRIVATE_SLOT(d_func(), void _q_updaterStateChanged(int, QProcess::ProcessState state))
	Q_PRIVATE_SLOT(d_func(), void _q_installerStateChanged(QProcess::ProcessState state))
};

}

#endif

#endif // QTAUTOUPDATER_PROCESSBACKEND_H
