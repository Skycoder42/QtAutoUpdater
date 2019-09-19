#include "qtestupdaterbackend.h"
#include "qtestupdateinstaller.h"
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
										   Feature::ParallelTrigger |
										   Feature::PerformInstall;
	return static_cast<Features>(config()->value(QStringLiteral("features"), DefaultFeatures).toInt());
}

UpdaterBackend::SecondaryInfo QTestUpdaterBackend::secondaryInfo() const
{
	auto key = config()->value(QStringLiteral("secondaryInfo/key"));
	if (key)
		return std::make_pair(key->toString(), config()->value(QStringLiteral("secondaryInfo/displayName"), key->toString()).toString());
	else
		return std::nullopt;
}

void QTestUpdaterBackend::checkForUpdates()
{
	qCDebug(logTestPlugin) << Q_FUNC_INFO;
	const milliseconds delay {config()->value(QStringLiteral("delay"), 200).toInt()};
	_tCounter = 0;
	if (features().testFlag(Feature::CheckProgress))
		emit checkProgress(0.0, config()->value(QStringLiteral("status"), {}).toString());
	_timer->start(delay);
}

void QTestUpdaterBackend::abort(bool force)
{
	qCDebug(logTestPlugin) << Q_FUNC_INFO << force;
	const auto abortLevel = config()->value(QStringLiteral("abortLevel"), 0).toInt();
	switch (abortLevel) {
	case 0:
		if (!force) {
			QTimer::singleShot(config()->value(QStringLiteral("cancelDelay"), 0).toInt(), this, [this](){
				if (_timer->isActive()) {
					_timer->stop();
					emit checkDone(true, {});
				}
			});
			break;
		}
		Q_FALLTHROUGH();
	case 1:
		if (force) {
			_timer->stop();
			emit checkDone(true, {});
		}
		break;
	case 2:
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

bool QTestUpdaterBackend::triggerUpdates(const QList<UpdateInfo> &infos, bool track)
{
	qCDebug(logTestPlugin) << Q_FUNC_INFO << infos << track;
	if (!features().testFlag(Feature::TriggerInstall))
		return false;

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
	qCDebug(logTestPlugin) << Q_FUNC_INFO;
	if (!features().testFlag(Feature::PerformInstall))
		return nullptr;
	return new QTestUpdateInstaller{config(), this};
}

bool QTestUpdaterBackend::initialize()
{
	qCDebug(logTestPlugin) << Q_FUNC_INFO;
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
		++_tCounter;
		if (features().testFlag(Feature::CheckProgress))
			emit checkProgress(_tCounter / 10.0, config()->value(QStringLiteral("status"), {}).toString());
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
					const auto dataKeys = readStringList(config()->value(QStringLiteral("updates/%1/data").arg(i), {}));
					for (const auto &key : dataKeys)
						info.setData(key, config()->value(QStringLiteral("updates/%1/%2").arg(i).arg(key), {}));
					updates.append(info);
				}
			}
			emit checkDone(success, updates);
		}
	}
}
