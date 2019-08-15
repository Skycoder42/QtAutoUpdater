#include "qtifwupdaterplugin.h"
#include "qtifwupdaterbackend.h"
using namespace QtAutoUpdater;

QtIfwUpdaterPlugin::QtIfwUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QtIfwUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("qtifw"))
		return new QtIfwUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
