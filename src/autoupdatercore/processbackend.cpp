#include "processbackend.h"
#include "processbackend_p.h"
#include "adminauthoriser.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
using namespace QtAutoUpdater;
using namespace std::placeholders;

QT_REQUIRE_CONFIG(process);

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logProcessBackend, "qt.autoupdater.core.ProcessBackend")

}

void ProcessBackend::abort(bool force)
{
	Q_D(ProcessBackend);
	for (const auto &pInfo : d->updateProcesses) {
		if (pInfo.second->state() != QProcess::NotRunning) {
			if (force)
				pInfo.second->kill();
			else
				pInfo.second->terminate();
		}
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

void ProcessBackend::runUpdateTool(int id, ProcessBackend::UpdateProcessInfo toolInfo)
{
	Q_D(ProcessBackend);

	auto proc = d->createProc(toolInfo);
	if (!toolInfo.useStdout)
		proc->setStandardOutputFile(QProcess::nullDevice());
	if (!toolInfo.useStderr)
		proc->setProcessChannelMode(QProcess::ForwardedErrorChannel);
	if (!toolInfo.stdinData)
		proc->setStandardInputFile(QProcess::nullDevice());
	connect(proc, &QProcess::stateChanged,
			this, std::bind(&ProcessBackendPrivate::_q_updaterStateChanged, d, id, _1));

	d->updateProcesses[id].first = std::move(toolInfo);
	d->updateProcesses[id].second = proc;
	qCDebug(logProcessBackend) << "Starting process for id" << id
							   << "as" << d->updateProcesses[id].second->program()
							   << "with arguments" << d->updateProcesses[id].second->arguments()
							   << "and working dir" << d->updateProcesses[id].second->workingDirectory();
	proc->start(d->updateProcesses[id].first.stdinData ?
					QIODevice::ReadWrite :
					QIODevice::ReadOnly);
}

void ProcessBackend::cancelUpdateTool(int id, bool kill)
{
	Q_D(ProcessBackend);
	if (!d->updateProcesses.contains(id))
		return;

	if (kill)
		d->updateProcesses[id].second->kill();
	else
		d->updateProcesses[id].second->terminate();
}

QStringList ProcessBackend::readPathList(const QVariant &value)
{
	return readStringList(value, QDir::listSeparator());
}

QStringList ProcessBackend::readArgumentList(const QVariant &value)
{
	if (value.userType() == QMetaType::QStringList ||
		value.userType() == QMetaType::QVariantList)
		return value.toStringList();
	else {
		QStringList args;

		enum { None, Word, Quoted, QuotedSingle } state = None;
		QString current;
		bool escaped = false;
		for (const auto &c : value.toString()) {
			if (escaped) {
				current.append(c);
				escaped = false;
				if (state == None)
					state = Word;
			} else if (c == QLatin1Char('\\'))
				escaped = true;
			else {
				switch (state) {
				case None:
					if (!c.isSpace()) {
						if (c == QLatin1Char('"'))
							state = Quoted;
						else if (c == QLatin1Char('\''))
							state = QuotedSingle;
						else {
							current.append(c);
							state = Word;
						}
					}
					break;
				case Word:
					if (c.isSpace()) {
						args.append(current);
						current.clear();
						state = None;
					} else if (c == QLatin1Char('"'))
						state = Quoted;
					else if (c == QLatin1Char('\''))
						state = QuotedSingle;
					else
						current.append(c);
					break;
				case Quoted:
					if (c == QLatin1Char('"'))
						state = Word;
					else
						current.append(c);
					break;
				case QuotedSingle:
					if (c == QLatin1Char('\''))
						state = Word;
					else
						current.append(c);
					break;
				}
			}
		}
		if (state != None)
			args.append(current);

		return args;
	}
}

// ------------- private implementation -------------

ProcessBackendPrivate::ProcessBackendPrivate(QString &&pKey) :
	UpdaterBackendPrivate{std::move(pKey)}
{}

void ProcessBackendPrivate::_q_updaterStateChanged(int id, QProcess::ProcessState state)
{
	Q_Q(ProcessBackend);
	if (!updateProcesses.contains(id))
		return;

	const auto &info = updateProcesses[id];
	Q_ASSERT(info.second);
	qCDebug(logProcessBackend) << "Process for id" << id
							   << "changed to state" << state;
	switch (state) {
	case QProcess::Starting:
		break;
	case QProcess::Running:
		if (info.first.stdinData)
			info.second->write(*info.first.stdinData);
		break;
	case QProcess::NotRunning:
		switch (info.second->exitStatus()) {
		case QProcess::NormalExit:
			if (info.first.useStdout) {
				info.second->setReadChannel(QProcess::StandardOutput);
				q->onToolDone(id, info.second->exitCode(), info.second);
			} else if (info.first.useStderr) {
				info.second->setReadChannel(QProcess::StandardError);
				q->onToolDone(id, info.second->exitCode(), info.second);
			} else
				q->onToolDone(id, info.second->exitCode(), nullptr);
			break;
		case QProcess::CrashExit:
			qCWarning(logProcessBackend) << "Failed to run"
										 << info.first.program
										 << "with crash error:"
										 << qUtf8Printable(info.second->errorString());
			emit q->checkDone(false);
			break;
		}
		info.second->deleteLater();
		updateProcesses.remove(id);
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
