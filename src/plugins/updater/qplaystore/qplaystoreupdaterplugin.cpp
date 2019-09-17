#include "qplaystoreupdaterplugin.h"
#include "qplaystoreupdaterbackend.h"
#include <QtAndroidExtras/QAndroidJniEnvironment>

QPlayStoreUpdaterPlugin::QPlayStoreUpdaterPlugin(QObject *parent) :
	QObject{parent}
{
	QAndroidJniEnvironment env;
	QPlayStoreUpdaterBackend::registerNatives(env);
}

QtAutoUpdater::UpdaterBackend *QPlayStoreUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("playstore"))
		return new QPlayStoreUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
