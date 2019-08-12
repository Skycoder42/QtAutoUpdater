#include "qtifwupdaterplugin.h"
#include "qtifwupdaterbackend.h"
using namespace QtAutoUpdater;

QtIfwUpdaterPlugin::QtIfwUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QtIfwUpdaterPlugin::createInstance(const QString &type, QObject *parent)
{
	if (type == QStringLiteral("qtifw"))
		return new QtIfwUpdaterBackend{parent};
	else
		return nullptr;
}
