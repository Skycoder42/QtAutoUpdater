#include "qhomebrewupdaterplugin.h"
#include "qhomebrewupdaterbackend.h"

QHomebrewUpdaterPlugin::QHomebrewUpdaterPlugin(QObject *parent) :
	QObject{parent}
{
	qDebug() << Q_FUNC_INFO;
}

QtAutoUpdater::UpdaterBackend *QHomebrewUpdaterPlugin::create(QString key, QObject *parent)
{
	qDebug() << Q_FUNC_INFO << key << parent;
	if (key == QStringLiteral("homebrew"))
		return new QHomebrewUpdaterBackend{std::move(key), parent};
	else
		return nullptr;
}
