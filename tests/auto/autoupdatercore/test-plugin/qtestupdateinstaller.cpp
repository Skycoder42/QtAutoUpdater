#include "qtestupdateinstaller.h"
using namespace QtAutoUpdater;

QTestUpdateInstaller::QTestUpdateInstaller(UpdaterBackend::IConfigReader *config, QObject *parent) :
	UpdateInstaller{parent},
	_config{config},
	_updateTimer{new QTimer{this}}
{
	connect(_updateTimer, &QTimer::timeout,
			this, &QTestUpdateInstaller::doInstallStep);
}

UpdateInstaller::Features QTestUpdateInstaller::features() const
{
	static constexpr int DefaultFeatures = Feature::DetailedProgress |
										   Feature::SelectComponents;
	return static_cast<Features>(_config->value(QStringLiteral("installer/features"), DefaultFeatures).toInt());
}

void QTestUpdateInstaller::startInstall()
{
	const auto eCnt = _config->value(QStringLiteral("eulas/size"), 0).toInt();
	for (auto i = 0; i < eCnt; ++i) {
		const auto required = _config->value(QStringLiteral("eulas/%i/required").arg(i), true).toBool();
		if (required)
			++_openEulas;
		emit showEula(QUuid::createUuid(),
					  _config->value(QStringLiteral("eulas/%i/text").arg(i), {}).toString(),
					  required);
	}

	if (_openEulas == 0)
		doInstall();
}

void QTestUpdateInstaller::eulaHandled(QUuid id, bool accepted)
{
	if (accepted) {
		if (--_openEulas == 0)
			doInstall();
	} else
		emit installFailed(QStringLiteral("EULA_%1_rejected").arg(id.toString(QUuid::WithoutBraces)));
}

void QTestUpdateInstaller::doInstallStep()
{
	// update progress
	auto cCnt = components().size();
	auto cComp = components().at(_currentIndex);
	emit updateComponentProgress(cComp.identifier(), ++_currentProgress / 10.0);
	emit updateGlobalProgress((_currentIndex * 10.0 + _currentProgress) / (cCnt * 10.0),
							  _config->value(QStringLiteral("installer/status"), {}).toString());

	// handle component rollover
	if (_currentProgress == 10) {
		_currentProgress = 0;
		if (++_currentIndex == cCnt) {
			_updateTimer->stop();
			if (_config->value(QStringLiteral("installer/success"), true).toBool())
				emit installSucceeded(_config->value(QStringLiteral("installer/restart"), false).toBool());
			else
				emit installFailed(QStringLiteral("installer/success"));
		}
	}
}

void QTestUpdateInstaller::doInstall()
{
	emit updateGlobalProgress(0.0, _config->value(QStringLiteral("installer/status"), {}).toString());
	_updateTimer->start(_config->value(QStringLiteral("installer/delay"), 200).toInt());
}
