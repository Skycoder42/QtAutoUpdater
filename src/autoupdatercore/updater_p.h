#ifndef QTAUTOUPDATER_UPDATER_P_H
#define QTAUTOUPDATER_UPDATER_P_H

#include "qtautoupdatercore_global.h"
#include "updater.h"
#include "simplescheduler_p.h"
#include "updaterbackend.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QSettings>
#include <QtCore/QScopedPointer>

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERCORE_EXPORT UpdaterPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(Updater)

public:
	using State = Updater::State;
	using InstallModeFlag = Updater::InstallModeFlag;
	using InstallMode = Updater::InstallMode;
	using InstallScope = Updater::InstallScope;

	enum class InstallerType {
		None,
		Perform,
		Trigger,
		OnExit
	};

	static InstallerType calcInstallerType(InstallMode mode,
										   InstallScope scope,
										   UpdaterBackend::Features features,
										   const QString &key);

	State state = State::NoUpdates;
	QList<UpdateInfo> updateInfos;

	SimpleScheduler *scheduler = nullptr;
	UpdaterBackend *backend = nullptr;

	bool runOnExit = false;

	static QSettings *findDefaultConfig();
	static Updater *createUpdater(UpdaterBackend::IConfigReader *config,
								  QObject *parent);

	void setupBackend(UpdaterBackend *newBackend);
	void _q_appAboutToExit();
	void _q_checkDone(bool success, QList<UpdateInfo> updates);
	void _q_triggerInstallDone(bool success);
};

class Q_AUTOUPDATERCORE_EXPORT VariantConfigReader : public UpdaterBackend::IConfigReader
{
public:
	VariantConfigReader(QString &&backend, QVariantMap &&map);

	QString backend() const override;
	std::optional<QVariant> value(const QString &key) const override;
	QVariant value(const QString &key, const QVariant &defaultValue) const override;

private:
	QString _backend;
	QVariantMap _map;
};

class Q_AUTOUPDATERCORE_EXPORT SettingsConfigReader : public UpdaterBackend::IConfigReader
{
public:
	SettingsConfigReader(QSettings *settings);

	QString backend() const override;
	std::optional<QVariant> value(const QString &key) const override;
	QVariant value(const QString &key, const QVariant &defaultValue) const override;

private:
	QScopedPointer<QSettings, QScopedPointerDeleteLater> _settings;
};

Q_DECLARE_LOGGING_CATEGORY(logUpdater)

}

#endif // QTAUTOUPDATER_UPDATER_P_H
