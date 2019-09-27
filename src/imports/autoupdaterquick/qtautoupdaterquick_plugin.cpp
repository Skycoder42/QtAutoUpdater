#include "qtautoupdaterquick_plugin.h"
#include <QtQml>

#include "qmlupdateinfomodel.h"
#include "qmlautoupdaterquickhelper.h"
using namespace QtAutoUpdater;

namespace {

QObject *create_qtautoupdaterquick(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
	Q_UNUSED(jsEngine)
	return new QmlAutoUpdaterQuickHelper{qmlEngine};
}

}

QtAutoUpdaterQuickDeclarativeModule::QtAutoUpdaterQuickDeclarativeModule(QObject *parent) :
	QQmlExtensionPlugin{parent}
{}

void QtAutoUpdaterQuickDeclarativeModule::registerTypes(const char *uri)
{
	Q_ASSERT(qstrcmp(uri, "de.skycoder42.QtAutoUpdater.Quick") == 0);

	qmlRegisterType<QmlUpdateInfoModel>(uri, 3, 0, "UpdateInfoModel");

	qmlRegisterSingletonType<QmlAutoUpdaterQuickHelper>("de.skycoder42.QtAutoUpdater.Quick.internal", 3, 0, "QtAutoUpdaterQuick", create_qtautoupdaterquick);

	static_assert(VERSION_MAJOR == 3 && VERSION_MINOR == 0, "QML module version needs to be updated");
}
