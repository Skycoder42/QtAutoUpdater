#include "qmlautoupdaterquickhelper.h"
#include <QtQml/QQmlEngine>
using namespace QtAutoUpdater;

QmlAutoUpdaterQuickHelper::QmlAutoUpdaterQuickHelper(QObject *parent) :
	QObject{parent}
{}

void QmlAutoUpdaterQuickHelper::jsOwn(QObject *object) const
{
	QQmlEngine::setObjectOwnership(object, QQmlEngine::JavaScriptOwnership);
}

void QmlAutoUpdaterQuickHelper::destroy(QObject *object) const
{
	Q_ASSERT(QQmlEngine::objectOwnership(object) == QQmlEngine::CppOwnership);
	QMetaObject::invokeMethod(object, "deleteLater", Qt::QueuedConnection);
}

QString QmlAutoUpdaterQuickHelper::secondaryInfoKey(QtAutoUpdater::Updater *updater) const
{
	if (const auto info = updater->backend()->secondaryInfo(); info)
		return info->first;
	else
		return {};
}

QString QmlAutoUpdaterQuickHelper::secondaryInfoDisplayName(QtAutoUpdater::Updater *updater) const
{
	if (const auto info = updater->backend()->secondaryInfo(); info)
		return info->second;
	else
		return {};
}
