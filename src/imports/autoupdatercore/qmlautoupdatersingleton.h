#ifndef QMLAUTOUPDATERSINGLETON_H
#define QMLAUTOUPDATERSINGLETON_H

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QPointer>

#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtAutoUpdaterCore/Updater>
#include <QtAutoUpdaterCore/UpdaterBackend>

class QmlAutoUpdaterSingleton : public QObject
{
	Q_OBJECT

public:
	explicit QmlAutoUpdaterSingleton(QObject *parent = nullptr);

	Q_INVOKABLE QtAutoUpdater::UpdateInfo createInfo(QVariant identifier, QString name, QVersionNumber version, QVariantMap data = {}) const;
	Q_INVOKABLE QVariantList infosAsList(const QList<QtAutoUpdater::UpdateInfo> &infos) const;


	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(const QString &configPath,
													  QObject *parent = nullptr) const;
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QSettings *config,
													  QObject *parent = nullptr) const;
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QString key,
													  QVariantMap arguments,
													  QObject *parent = nullptr) const;
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QObject *qmlConfigOrParent = nullptr) const;
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QObject *qmlConfig,
													  QObject *parent) const;

private:
	class QmlConfigReader : public QtAutoUpdater::UpdaterBackend::IConfigReader
	{
	public:
		QmlConfigReader(QObject *qmlSettings);

		QString backend() const override;
		std::optional<QVariant> value(const QString &key) const override;
		QVariant value(const QString &key, const QVariant &defaultValue) const override;

	private:
		QPointer<QObject> _qmlSettings;
	};
};

#endif // QMLAUTOUPDATERSINGLETON_H
