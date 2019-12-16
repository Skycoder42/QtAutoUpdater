#ifndef QMLAUTOUPDATERSINGLETON_H
#define QMLAUTOUPDATERSINGLETON_H

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QPointer>

#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtAutoUpdaterCore/Updater>
#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtQml/QJSValue>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtAutoUpdater::Core {

/*! @brief A singleton class to create instances of different QtAutoUpdater types
 *
 * @extends QtQml.QtObject
 * @since 3.0
 */
class QtAutoUpdater
#else
namespace QtAutoUpdater {

class QmlAutoUpdaterSingleton : public QObject
#endif
{
	Q_OBJECT

public:
	//! @private
	explicit QmlAutoUpdaterSingleton(QObject *parent = nullptr);

	//! Creates a QtAutoUpdater::UpdateInfo from the given parameters
	Q_INVOKABLE QVariant createInfo(QVariant identifier, QString name, const QVariant &version, QVariantMap data = {}) const;
	//! Converts a list of QtAutoUpdater::UpdateInfos to a QVariantList, which can be consumed in QML
	Q_INVOKABLE Q_DECL_DEPRECATED QJSValue infosAsList(const QJSValue &value) const;


	//! @copydoc QtAutoUpdater::Updater::create(const QString &, QObject*)
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(const QString &configPath,
													  QObject *parent = nullptr) const;
	//! @copydoc QtAutoUpdater::Updater::create(QSettings *, QObject*)
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QSettings *config,
													  QObject *parent = nullptr) const;
	//! @copydoc QtAutoUpdater::Updater::create(QString, QVariantMap, QObject*)
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QString key,
													  QVariantMap configuration,
													  QObject *parent = nullptr) const;
	/*! @brief Creates an updater instance using the given object as either QML configuration or as parent
	 *
	 * @param qmlConfigOrParent The object to be consumed
	 * @returns A newly created updater instance or `nullptr`, if unable to create one.
	 *
	 * The qmlConfigOrParent can be interpreted in different ways. If it is a Qt.labs.settings.Settings instance,
	 * it is used as the configuration, but not as the parent. Otherwise it is used as the parent.
	 *
	 * The backend is loaded based on the given configuration. It must contain the "backend" entry, set to
	 * the updater backend to be used. If the backend is missing or invalid, nullptr is returned. Otherwise
	 * the created instance, initialized using the given configuration.
	 *
	 * @attention For some update installations, an updater needs to be launched on exit of the running
	 * application. This only works if the updater lives long enough to catch the
	 * QCoreApplication::aboutToQuit signal! This means when creating an updater for a backend that may need
	 * to be run on exit, you should use either `qApp` or `nullptr` as parent for the updater.
	 *
	 * @sa Updater::supportedUpdaterBackends, @ref qtautoupdater_backends "Updater Backend Plugins", Qt.labs.settings.Settings
	 */
	Q_INVOKABLE QtAutoUpdater::Updater *createUpdater(QObject *qmlConfigOrParent = nullptr) const;
	/*! @brief Creates an updater instance using the given QML configuration and parent
	 *
	 * @param qmlConfig The configuration object
	 * @param parent The object to be set as the parent for the created updater
	 * @returns A newly created updater instance or `nullptr`, if unable to create one.
	 *
	 * The qmlConfig must be an instance of Qt.labs.settings.Settings or QSettings. If it is neither, no updater
	 * can be created an nullptr gets returned
	 *
	 * The backend is loaded based on the given configuration. It must contain the "backend" entry, set to
	 * the updater backend to be used. If the backend is missing or invalid, nullptr is returned. Otherwise
	 * the created instance, initialized using the given configuration.
	 *
	 * @attention For some update installations, an updater needs to be launched on exit of the running
	 * application. This only works if the updater lives long enough to catch the
	 * QCoreApplication::aboutToQuit signal! This means when creating an updater for a backend that may need
	 * to be run on exit, you should use either `qApp` or `nullptr` as parent for the updater.
	 *
	 * @sa Updater::supportedUpdaterBackends, @ref qtautoupdater_backends "Updater Backend Plugins", Qt.labs.settings.Settings,
	 * QSettings
	 */
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

}

#endif // QMLAUTOUPDATERSINGLETON_H
