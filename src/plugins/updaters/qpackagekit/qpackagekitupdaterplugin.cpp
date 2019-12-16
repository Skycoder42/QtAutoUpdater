#include "qpackagekitupdaterplugin.h"
#include "qpackagekitupdaterbackend.h"

QPackageKitUpdaterPlugin::QPackageKitUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QPackageKitUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("packagekit"))
		return new QPackageKitUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
