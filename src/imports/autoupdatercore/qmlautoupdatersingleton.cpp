#include "qmlautoupdatersingleton.h"
#include <QtQml>
using namespace QtAutoUpdater;

QmlAutoUpdaterSingleton::QmlAutoUpdaterSingleton(QObject *parent) :
	QObject{parent}
{}

UpdateInfo QmlAutoUpdaterSingleton::createInfo(QString name, QVersionNumber version, quint64 size, QVariant identifier) const
{
	return {std::move(name), std::move(version), size, std::move(identifier)};
}

QVariantList QmlAutoUpdaterSingleton::infosAsList(const QList<UpdateInfo> &infos) const
{
	QVariantList l;
	l.reserve(infos.size());
	for (const auto &info : infos)
		l.append(QVariant::fromValue(info));
	return l;
}

Updater *QmlAutoUpdaterSingleton::createUpdater(const QString &configPath, QObject *parent) const
{
	return Updater::create(configPath, parent);
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QSettings *config, QObject *parent) const
{
	return Updater::create(config, parent);
}

Updater *QmlAutoUpdaterSingleton::createUpdater(QString key, QVariantMap arguments, QObject *parent) const
{
	return Updater::create(std::move(key), std::move(arguments), parent);
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
	if (QMetaObject::invokeMethod(_qmlSettings, "value", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, res),
								  Q_ARG(QString, key))) {
		if (res.isValid())
			return res;
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

QVariant QmlAutoUpdaterSingleton::QmlConfigReader::value(const QString &key, const QVariant &defaultValue) const
{
	QVariant res;
	if (QMetaObject::invokeMethod(_qmlSettings, "value", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, res),
								  Q_ARG(QString, key),
								  Q_ARG(QVariant, defaultValue)))
		return res;
	else
		return {};
}
