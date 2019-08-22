#include "qtestupdaterplugin.h"
#include "qtestupdaterbackend.h"

QTestUpdaterPlugin::QTestUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QTestUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("test"))
		return new QTestUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}

