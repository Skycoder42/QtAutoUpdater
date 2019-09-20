#include "qhomebrewupdaterplugin.h"
#include "qhomebrewupdaterbackend.h"

QHomebrewUpdaterPlugin::QHomebrewUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QHomebrewUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("homebrew"))
		return new QHomebrewUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
