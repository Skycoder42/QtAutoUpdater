#include "processbackend.h"
#include "processbackend_p.h"
#include "adminauthoriser.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
using namespace QtAutoUpdater;

QT_REQUIRE_CONFIG(process);

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logProcessBackend, "qt.autoupdater.core.ProcessBackend")

}

void ProcessBackend::checkForUpdates()
{
	Q_D(ProcessBackend);
	if (d->updateProc->state() == QProcess::NotRunning)
		d->updateProc->start(d->procInfo.stdinData ? QIODevice::ReadWrite : QIODevice::ReadOnly);
}

void ProcessBackend::abort(bool force)
{
	Q_D(ProcessBackend);
	if (d->updateProc->state() != QProcess::NotRunning) {
		if (force)
			d->updateProc->kill();
		else
			d->updateProc->terminate();
	}
}

bool ProcessBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
	Q_D(ProcessBackend);
	if (d->installProc) {
		qCWarning(logProcessBackend) << "Another installer process is still running";
		return false;
	}

	auto procInfo = installerInfo(infos, track);
	if (!procInfo)
		return false;

	if (!procInfo->runAsAdmin)
		procInfo->runAsAdmin = AdminAuthoriser::needsAdminPermission(procInfo->program);

	if (*procInfo->runAsAdmin) {
		if (track)
			qCWarning(logProcessBackend) << "Unable to track progress of application executed as root/admin! It will be run detached instead";
		const auto ok = AdminAuthoriser::executeAsAdmin(procInfo->program,
														procInfo->arguments,
														procInfo->workingDir.value_or(QFileInfo{procInfo->program}.dir().absolutePath()));
		if (ok && track) { // invoke queued to make shure is emitted AFTER the start install signal in the updater
			QMetaObject::invokeMethod(this, "triggerInstallDone", Qt::QueuedConnection,
									  Q_ARG(bool, true));
		}
		return ok;
	} else {
		d->installProc = d->createProc(*procInfo);
		if (track) {
			d->installProc->setProcessChannelMode(QProcess::ForwardedChannels);
			d->installProc->setInputChannelMode(QProcess::ForwardedInputChannel);
			QObjectPrivate::connect(d->installProc, &QProcess::stateChanged,
									d, &ProcessBackendPrivate::_q_installerStateChanged);
			d->installProc->start(QIODevice::ReadWrite);
			return true;
		} else {
			auto ok = d->installProc->startDetached();
			if (!ok)
				qCCritical(logProcessBackend) << "Failed to start" << procInfo->program << "to install updates";
			d->installProc->deleteLater();
			d->installProc.clear();
			return ok;
		}
	}
}

ProcessBackend::ProcessBackend(QString &&key, QObject *parent) :
	ProcessBackend{*new ProcessBackendPrivate{std::move(key)}, parent}
{}

ProcessBackend::ProcessBackend(ProcessBackendPrivate &dd, QObject *parent) :
	UpdaterBackend{dd, parent}
{}

bool ProcessBackend::initialize()
{
	Q_D(ProcessBackend);
	auto info = initializeImpl();
	if (!info)
		return false;

	d->procInfo = *std::move(info);
	d->updateProc = d->createProc(d->procInfo);
	if (!d->procInfo.useStdout)
		d->updateProc->setStandardOutputFile(QProcess::nullDevice());
	if (!d->procInfo.useStderr)
		d->updateProc->setProcessChannelMode(QProcess::ForwardedErrorChannel);
	if (!d->procInfo.stdinData)
		d->updateProc->setStandardInputFile(QProcess::nullDevice());
	QObjectPrivate::connect(d->updateProc, &QProcess::stateChanged,
							d, &ProcessBackendPrivate::_q_updaterStateChanged);

	return true;
}

// ------------- private implementation -------------

ProcessBackendPrivate::ProcessBackendPrivate(QString &&pKey) :
	UpdaterBackendPrivate{std::move(pKey)}
{}

void ProcessBackendPrivate::_q_updaterStateChanged(QProcess::ProcessState state)
{
	Q_Q(ProcessBackend);
	switch (state) {
	case QProcess::Starting:
		break;
	case QProcess::Running:
		if (procInfo.stdinData)
			updateProc->write(*procInfo.stdinData);
		break;
	case QProcess::NotRunning:
		switch (updateProc->exitStatus()) {
		case QProcess::NormalExit:
			if (procInfo.useStdout) {
				updateProc->setReadChannel(QProcess::StandardOutput);
				q->parseResult(updateProc->exitCode(), updateProc);
			} else if (procInfo.useStderr) {
				updateProc->setReadChannel(QProcess::StandardError);
				q->parseResult(updateProc->exitCode(), updateProc);
			} else
				q->parseResult(updateProc->exitCode(), nullptr);
			break;
		case QProcess::CrashExit:
			qCWarning(logProcessBackend) << "Failed to run"
										 << procInfo.program
										 << "with crash error:"
										 << qUtf8Printable(updateProc->errorString());
			emit q->checkDone(false);
			break;
		}
		updateProc->close();
		break;
	}
}

void ProcessBackendPrivate::_q_installerStateChanged(QProcess::ProcessState state)
{
	Q_Q(ProcessBackend);
	switch (state) {
	case QProcess::NotRunning:
		switch (installProc->exitStatus()) {
		case QProcess::NormalExit:
			if (installProc->exitCode() == EXIT_SUCCESS)
				emit q->triggerInstallDone(true);
			else {
				qCWarning(logProcessBackend) << "Failed to run"
											 << installProc->program()
											 << "with exit code:"
											 << installProc->exitCode();
				emit q->triggerInstallDone(false);
			}
			break;
		case QProcess::CrashExit:
			qCWarning(logProcessBackend) << "Failed to run"
										 << installProc->program()
										 << "with crash error:"
										 << qUtf8Printable(installProc->errorString());
			emit q->triggerInstallDone(false);
			break;
		}
		disconnect(installProc, &QProcess::stateChanged,
				   this, &ProcessBackendPrivate::_q_installerStateChanged);
		installProc->close();
		installProc->deleteLater();
		installProc.clear();
		break;
	default:
		break;
	}
}

QProcess *ProcessBackendPrivate::createProc(const ProcessBackendPrivate::ProcessInfoBase &info)
{
	Q_Q(ProcessBackend);
	auto proc = new QProcess{q};
	proc->setProgram(info.program);
	proc->setArguments(info.arguments);
	proc->setWorkingDirectory(info.workingDir.value_or(QFileInfo{info.program}.dir().absolutePath()));
	return proc;
}

#include "moc_processbackend.cpp"
