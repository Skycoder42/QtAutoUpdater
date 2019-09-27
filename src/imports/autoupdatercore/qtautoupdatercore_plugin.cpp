#include "qtautoupdatercore_plugin.h"
#include <QtQml>

#include <QtAutoUpdaterCore/UpdateInfo>
#include <QtAutoUpdaterCore/UpdaterBackend>
#include <QtAutoUpdaterCore/UpdateInstaller>
#include <QtAutoUpdaterCore/Updater>

#include "qmlautoupdatersingleton.h"
using namespace QtAutoUpdater;

namespace {

QObject *create_qtautoupdater(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
	return new QmlAutoUpdaterSingleton{qmlEngine};
}

}

QtAutoUpdaterCoreDeclarativeModule::QtAutoUpdaterCoreDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin{parent}
{}

void QtAutoUpdaterCoreDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtAutoUpdater.Core") == 0);

	qmlRegisterUncreatableType<UpdateInfo>(uri, 3, 0, "UpdateInfo", QStringLiteral("Must be created by using the QtAutoUpdater singleton"));
	qmlRegisterUncreatableType<UpdaterBackend>(uri, 3, 0, "UpdaterBackend", QStringLiteral("Cannot be created, only obtained via Updater"));
	qmlRegisterUncreatableType<UpdateInstaller>(uri, 3, 0, "UpdateInstaller", QStringLiteral("Cannot be created, only obtained via Updater"));
	qmlRegisterUncreatableType<Updater>(uri, 3, 0, "Updater", QStringLiteral("Must be created by using the QtAutoUpdater singleton"));

	qmlRegisterSingletonType<QmlAutoUpdaterSingleton>(uri, 3, 0, "QtAutoUpdater", create_qtautoupdater);

	static_assert(VERSION_MAJOR == 3 && VERSION_MINOR == 0, "QML module version needs to be updated");
}
