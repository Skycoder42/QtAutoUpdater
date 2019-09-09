#include "updaterbackend.h"
#include "updaterbackend_p.h"
using namespace QtAutoUpdater;

UpdaterBackend::UpdaterBackend(QString &&key, QObject *parent) :
	UpdaterBackend{*new UpdaterBackendPrivate{std::move(key)}, parent}
{}

UpdaterBackend::UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{}

UpdaterBackend::IConfigReader *UpdaterBackend::config() const
{
	Q_D(const UpdaterBackend);
	return d->config.data();
}

QString UpdaterBackend::key() const
{
	Q_D(const UpdaterBackend);
	return d->key;
}

bool UpdaterBackend::initialize(QScopedPointer<UpdaterBackend::IConfigReader> &&config)
{
	Q_D(UpdaterBackend);
	d->config.swap(config);
	return initialize();
}

// ------------- private implementation -------------

UpdaterBackendPrivate::UpdaterBackendPrivate(QString &&pKey) :
	key{std::move(pKey)}
{}
