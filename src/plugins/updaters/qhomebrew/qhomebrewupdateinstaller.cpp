#include "qhomebrewupdateinstaller.h"
#include "qhomebrewupdaterbackend.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtAutoUpdaterCore/ProcessBackend>
using namespace QtAutoUpdater;

Q_LOGGING_CATEGORY(logBrewInstaller, "qt.autoupdater.core.plugin.homebrew.installer")

QHomebrewUpdateInstaller::QHomebrewUpdateInstaller(const QString &brewPath, UpdaterBackend::IConfigReader *config, QObject *parent) :
	UpdateInstaller{parent},
	_config{config},
	_installProcess{new QProcess{this}}
{
	_installProcess->setProgram(brewPath);
	_installProcess->setProcessChannelMode(QProcess::ForwardedErrorChannel);
	_installProcess->setStandardOutputFile(QProcess::nullDevice());
	_installProcess->setStandardInputFile(QProcess::nullDevice());
	_installProcess->setReadChannel(QProcess::StandardOutput);
	connect(_installProcess, &QProcess::stateChanged,
			this, &QHomebrewUpdateInstaller::stateChanged);
}

UpdateInstaller::Features QHomebrewUpdateInstaller::features() const
{
	return Feature::SelectComponents;
}

void QHomebrewUpdateInstaller::eulaHandled(const QVariant &id, bool accepted)
{
	Q_UNUSED(id)
	Q_UNUSED(accepted)
}

void QHomebrewUpdateInstaller::restartApplication()
{
	if (QProcess::startDetached(QCoreApplication::applicationFilePath(),
								QCoreApplication::arguments(),
								QDir::currentPath()))
		QCoreApplication::quit();
	else
		qCCritical(logBrewInstaller) << "Failed to restart the application";
}

void QHomebrewUpdateInstaller::startInstallImpl()
{
	QStringList args {
		QStringLiteral("upgrade")
	};
	if (_config->value(QHomebrewUpdaterBackend::KeyCask, QHomebrewUpdaterBackend::DefaultCask).toBool())
		args.append(QStringLiteral("cask"));
	if (auto extraArgs = _config->value(QHomebrewUpdaterBackend::KeyExtraInstallArgs); extraArgs)
		args += ProcessBackend::readArgumentList(*extraArgs);
	for (const auto &info : components())
		args.append(info.identifier().toString());
	_installProcess->setArguments(args);

	_installProcess->start(QIODevice::ReadOnly);
}

void QHomebrewUpdateInstaller::stateChanged(QProcess::ProcessState state)
{
	switch (state) {
	case QProcess::Starting:
		emit updateGlobalProgress(-1.0, tr("Starting homebrew…"));
		break;
	case QProcess::Running:
		emit updateGlobalProgress(-1.0, tr("Running homebrew in the background to perform the upgrade…"));
		break;
	case QProcess::NotRunning:
		switch (_installProcess->exitStatus()) {
		case QProcess::NormalExit:
			if (const auto exitCode = _installProcess->exitCode(); exitCode != EXIT_SUCCESS)
				emit installFailed(tr("Homebrew exited with unclean exit code %1").arg(exitCode));
			else
				emit installSucceeded(true);
			break;
		case QProcess::CrashExit:
			emit installFailed(_installProcess->errorString());
			break;
		}
		break;
	}
}
