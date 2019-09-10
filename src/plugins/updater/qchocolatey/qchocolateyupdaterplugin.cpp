#include "qchocolateyupdaterplugin.h"
#include "qchocolateyupdaterbackend.h"
using namespace QtAutoUpdater;

QChocolateyUpdaterPlugin::QChocolateyUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

UpdaterBackend *QChocolateyUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("chocolatey"))
		return new QChocolateyUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
