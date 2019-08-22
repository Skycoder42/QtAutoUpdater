#include "updateinfo.h"
#include "updateinfo_p.h"
#include <QtCore/QtDebug>
using namespace QtAutoUpdater;

namespace {

uint qHashImpl(const UpdateInfo &info, uint seed)
{
	return qHash(info.name(), seed) ^
			qHash(info.version(), seed) ^
			qHash(info.size(), seed);
}

}

UpdateInfo::UpdateInfo() :
	UpdateInfo{new UpdateInfoPrivate{}}
{}

UpdateInfo::UpdateInfo(QString name, QVersionNumber version, quint64 size, QVariant identifier) :
	UpdateInfo{new UpdateInfoPrivate{{}, std::move(name), std::move(version), size, std::move(identifier)}}
{}

UpdateInfo::UpdateInfo(UpdateInfoPrivate *d_ptr) :
	d{d_ptr}
{}

UpdateInfo::UpdateInfo(const UpdateInfo &other) = default;

UpdateInfo::UpdateInfo(UpdateInfo &&other) noexcept = default;

UpdateInfo &UpdateInfo::operator=(const UpdateInfo &other) = default;

UpdateInfo &UpdateInfo::operator=(UpdateInfo &&other) noexcept = default;

UpdateInfo::~UpdateInfo() = default;

bool UpdateInfo::operator==(const UpdateInfo &other) const
{
	return d == other.d || (
				d->name == other.d->name &&
				d->version == other.d->version &&
				d->size == other.d->size &&
				d->identifier == other.d->identifier);
}

bool UpdateInfo::operator!=(const UpdateInfo &other) const
{
	return d != other.d && (
				d->name != other.d->name ||
				d->version != other.d->version ||
				d->size != other.d->size ||
				d->identifier != other.d->identifier);
}

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

void UpdateInfo::setIdentifier(QVariant identifier)
{
	d->identifier = std::move(identifier);
}

uint QtAutoUpdater::qHash(const UpdateInfo &info, uint seed)
{
	return qHashImpl(info, seed);
}

QDebug &QtAutoUpdater::operator<<(QDebug &debug, const UpdateInfo &info)
{
	QDebugStateSaver state{debug};
	debug.noquote() << QStringLiteral("{Name: \"%1\"; Version: %2; Size: %3}")
					   .arg(info.name(), info.version().toString())
					   .arg(info.size());
	return debug;
}
