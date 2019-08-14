#include "updaterbackend.h"
#include "updaterbackend_p.h"
using namespace QtAutoUpdater;

bool UpdaterBackend::initialize(QScopedPointer<UpdaterBackend::IConfigReader> &&config, QScopedPointer<AdminAuthoriser> &&authoriser)
{
	Q_D(UpdaterBackend);
	d->config.swap(config);
	d->authoriser.swap(authoriser);
	return initialize();
}

UpdaterBackend::UpdaterBackend(QObject *parent) :
	UpdaterBackend{*new UpdaterBackendPrivate{}, parent}
{}

UpdaterBackend::UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{}

UpdaterBackend::IConfigReader *UpdaterBackend::config() const
{
	const Q_D(UpdaterBackend);
	return d->config.data();
}

AdminAuthoriser *UpdaterBackend::authoriser() const
{
	const Q_D(UpdaterBackend);
	return d->authoriser.data();
}
