#include "qpackagekitupdaterplugin.h"
#include "qpackagekitupdaterbackend.h"

QPackageKitUpdaterPlugin::QPackageKitUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QPackageKitUpdaterPlugin::create(const QString &type, QObject *parent)
{
	if (type == QStringLiteral("packagekit"))
		return new QPackageKitUpdaterBackend{parent};
	else
		return nullptr;
}
