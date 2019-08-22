#include "qtestupdaterbackend.h"
#include <chrono>
using namespace QtAutoUpdater;
using namespace std::chrono;

Q_LOGGING_CATEGORY(logTestPlugin, "qt.autoupdater.core.plugin.test")

QTestUpdaterBackend::QTestUpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{std::move(key), parent},
	_timer{new QTimer{this}}
{
	connect(_timer, &QTimer::timeout,
			this, &QTestUpdaterBackend::timerTriggered);
}

UpdaterBackend::Features QTestUpdaterBackend::features() const
{
	static constexpr int DefaultFeatures = Feature::CheckProgress |
										   Feature::TriggerInstall |
										   Feature::ParallelInstall |
										   Feature::PerformInstall;
	return static_cast<Features>(config()->value(QStringLiteral("features"), DefaultFeatures).toInt());
}

void QTestUpdaterBackend::checkForUpdates()
{
	qCInfo(logTestPlugin) << Q_FUNC_INFO;
	const milliseconds delay {config()->value(QStringLiteral("delay"), 200).toInt()};
	_tCounter = 0;
	emit checkProgress(0.0, config()->value(QStringLiteral("status"), {}).toString());
	_timer->start(delay);
}

void QTestUpdaterBackend::abort(bool force)
{
	qCInfo(logTestPlugin) << Q_FUNC_INFO << force;
	const auto abortLevel = config()->value(QStringLiteral("abortLevel"), 0).toInt();
	switch (abortLevel) {
	case 2:
		break;
	case 1:
		if (force)
			Q_FALLTHROUGH();
		else
			break;
	case 0:
		_timer->stop();
		emit checkDone(true, {});
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

bool QTestUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
	qCInfo(logTestPlugin) << Q_FUNC_INFO << infos << track;
	const auto success = config()->value(QStringLiteral("allowTrigger"), true).toBool();
	if (!success)
		return false;

	if (track) {
		_updating = true;
		const milliseconds delay {config()->value(QStringLiteral("updateTime"), 5000).toInt()};
		_timer->start(delay);
	}

	return true;
}

UpdateInstaller *QTestUpdaterBackend::createInstaller()
{
	qCInfo(logTestPlugin) << Q_FUNC_INFO;
	return nullptr;
}

bool QTestUpdaterBackend::initialize()
{
	qCInfo(logTestPlugin) << Q_FUNC_INFO;
	return config()->value(QStringLiteral("allowInit"), true).toBool();
}

void QTestUpdaterBackend::timerTriggered()
{
	if (_updating) {
		_updating = false;
		_timer->stop();
		const auto success = !config()->value(QStringLiteral("updateHasError"), false).toBool();
		emit triggerInstallDone(success);
	} else {
		emit checkProgress(++_tCounter / 10.0, config()->value(QStringLiteral("status"), {}).toString());
		if (_tCounter == 10) {
			_tCounter = 0;
			_timer->stop();

			const auto success = !config()->value(QStringLiteral("hasError"), false).toBool();
			QList<UpdateInfo> updates;
			if (success) {
				const auto uCnt = config()->value(QStringLiteral("updates/size"), 0).toInt();
				updates.reserve(uCnt);
				for (auto i = 0; i < uCnt; ++i) {
					UpdateInfo info;
					info.setIdentifier(config()->value(QStringLiteral("updates/%1/id").arg(i), {}));
					info.setName(config()->value(QStringLiteral("updates/%1/name").arg(i), {}).toString());
					info.setVersion(QVersionNumber::fromString(config()->value(QStringLiteral("updates/%1/version").arg(i), {}).toString()));
					info.setSize(config()->value(QStringLiteral("updates/%1/size").arg(i), 0).toULongLong());
					updates.append(info);
				}
			}
			emit checkDone(success, updates);
		}
	}
}
