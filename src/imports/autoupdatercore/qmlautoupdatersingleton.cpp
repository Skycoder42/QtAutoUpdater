#include "qmlautoupdatersingleton.h"
#include <QtQml>
using namespace QtAutoUpdater;

QmlAutoUpdaterSingleton::QmlAutoUpdaterSingleton(QObject *parent) :
	QObject{parent}
{}

QVariant QmlAutoUpdaterSingleton::createInfo(QVariant identifier, QString name, const QVariant &version, QVariantMap data) const
{
	return QVariant::fromValue(UpdateInfo {
		std::move(identifier),
		std::move(name),
		version.canConvert(qMetaTypeId<QVersionNumber>()) ?
			version.value<QVersionNumber>() :
			QVersionNumber::fromString(version.toString()),
		std::move(data)
							   });
}

QJSValue QmlAutoUpdaterSingleton::infosAsList(const QJSValue &value) const
{
	return value;
}

Updater *QmlAutoUpdaterSingleton::createUpdater(const QString &configPath, QObject *parent) const
{
	return Updater::create(configPath, parent);
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QSettings *config, QObject *parent) const
{
	return Updater::create(config, parent);
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QString key, QVariantMap configuration, QObject *parent) const
{
	return Updater::create(std::move(key), std::move(configuration), parent);
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QObject *qmlConfigOrParent) const
{
	if (qmlConfigOrParent)
		return createUpdater(qmlConfigOrParent, nullptr);
	else
		return Updater::create();
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QObject *qmlConfigOrParent, QObject *parent) const
{
	if (qmlConfigOrParent->inherits("QQmlSettings"))
		return Updater::create(new QmlConfigReader {qmlConfigOrParent}, parent);
	else if (qmlConfigOrParent->metaObject()->inherits(&QSettings::staticMetaObject))
		return Updater::create(static_cast<QSettings*>(qmlConfigOrParent), parent);
	else if (parent) {
		qmlWarning(this) << "First argument is not a QSettings or QML Settings object!";
		return nullptr;
	} else
		return Updater::create(qmlConfigOrParent);
}



QmlAutoUpdaterSingleton::QmlConfigReader::QmlConfigReader(QObject *qmlSettings) :
	_qmlSettings{qmlSettings}
{}

QString QmlAutoUpdaterSingleton::QmlConfigReader::backend() const
{
	return value(QStringLiteral("backend"), {}).toString();
}

std::optional<QVariant> QmlAutoUpdaterSingleton::QmlConfigReader::value(const QString &key) const
{
	QVariant res;
	QQmlProperty prop {
		_qmlSettings,
		QString{key}.replace(QRegularExpression{QStringLiteral("\\W")}, QStringLiteral("_"))
	};
	if (prop.isValid())
		return prop.read();
	else
		return std::nullopt;
}

QVariant QmlAutoUpdaterSingleton::QmlConfigReader::value(const QString &key, const QVariant &defaultValue) const
{
	return value(key).value_or(defaultValue);
}
