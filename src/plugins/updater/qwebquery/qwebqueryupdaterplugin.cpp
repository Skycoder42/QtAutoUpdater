#include "qwebqueryupdaterplugin.h"
#include "qwebqueryupdaterbackend.h"

QWebQueryUpdaterPlugin::QWebQueryUpdaterPlugin(QObject *parent) :
	QObject{parent}
{}

QtAutoUpdater::UpdaterBackend *QWebQueryUpdaterPlugin::create(QString key, QObject *parent)
{
	if (key == QStringLiteral("webquery"))
		return new QWebQueryUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
