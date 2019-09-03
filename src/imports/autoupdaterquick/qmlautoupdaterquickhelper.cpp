#include "qmlautoupdaterquickhelper.h"
#include <QtQml/QQmlEngine>

QmlAutoUpdaterQuickHelper::QmlAutoUpdaterQuickHelper(QObject *parent) :
	QObject{parent}
{}

void QmlAutoUpdaterQuickHelper::jsOwn(QObject *object)
{
	QQmlEngine::setObjectOwnership(object, QQmlEngine::JavaScriptOwnership);
}

void QmlAutoUpdaterQuickHelper::destroy(QObject *object)
{
	Q_ASSERT(QQmlEngine::objectOwnership(object) == QQmlEngine::CppOwnership);
	QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
}
