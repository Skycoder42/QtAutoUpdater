#include "qplaystoreupdaterplugin.h"
#include "qplaystoreupdaterbackend.h"

QPlayStoreUpdaterPlugin::QPlayStoreUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QPlayStoreUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("playstore"))
		return new QPlayStoreUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
