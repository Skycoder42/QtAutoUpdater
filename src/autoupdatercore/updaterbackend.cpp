#include "updaterbackend.h"
#include "updaterbackend_p.h"
using namespace QtAutoUpdater;

UpdaterBackend::UpdaterBackend(QObject *parent) :
	UpdaterBackend{*new UpdaterBackendPrivate{}, parent}
{}

UpdaterBackend::UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{}

void UpdaterBackend::setProgress(double percent, const QString &status)
{
	emit updateProgress(percent, status, {});
}

void UpdaterBackend::setProgress(int value, int max, const QString &status)
{
	emit updateProgress(static_cast<double>(value) / static_cast<double>(max), status, {});
}



UpdateInfo::UpdateInfo() :
	UpdateInfo{new UpdateInfoPrivate{}}
{}

UpdateInfo::UpdateInfo(UpdateInfoPrivate *d_ptr) :
	d{d_ptr}
{}

UpdateInfo::~UpdateInfo() = default;

QString UpdateInfo::name() const
{
	return d->name;
}

QVersionNumber UpdateInfo::version() const
{
	return d->version;
}

quint64 UpdateInfo::size() const
{
	return d->size;
}

QVariant UpdateInfo::identifier() const
{
	return d->identifier;
}

void UpdateInfo::setName(QString name)
{
	d->name = std::move(name);
}

void UpdateInfo::setVersion(QVersionNumber version)
{
	d->version = std::move(version);
}

void UpdateInfo::setSize(quint64 size)
{
	d->size = size;
}
