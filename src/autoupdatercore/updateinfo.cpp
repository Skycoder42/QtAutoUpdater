#include "updateinfo.h"
#include "updateinfo_p.h"
#include <QtCore/QtDebug>
using namespace QtAutoUpdater;

namespace {

uint qHashImpl(const UpdateInfo &info, uint seed)
{
	return qHash(info.identifier().toString(), seed) ^
			qHash(info.name(), seed) ^
			qHash(info.version(), seed);
}

}

UpdateInfo::UpdateInfo() :
	UpdateInfo{new UpdateInfoPrivate{}}
{}

UpdateInfo::UpdateInfo(QVariant identifier, QString name, QVersionNumber version, QVariantMap data) :
	UpdateInfo{new UpdateInfoPrivate{{}, std::move(identifier), std::move(name), std::move(version), std::move(data)}}
{}

UpdateInfo::UpdateInfo(UpdateInfoPrivate *d_ptr) :
	d{d_ptr}
{
	static auto once = true;
	if (once) {
		once = false;
		qRegisterMetaType<UpdateInfo>();
		qRegisterMetaType<QList<UpdateInfo>>();
		QMetaType::registerEqualsComparator<UpdateInfo>();
	}
}

UpdateInfo::UpdateInfo(const UpdateInfo &other) = default;

UpdateInfo::UpdateInfo(UpdateInfo &&other) noexcept = default;

UpdateInfo &UpdateInfo::operator=(const UpdateInfo &other) = default;

UpdateInfo &UpdateInfo::operator=(UpdateInfo &&other) noexcept = default;

UpdateInfo::~UpdateInfo() = default;

bool UpdateInfo::operator==(const UpdateInfo &other) const
{
	return d == other.d || (
				d->identifier == other.d->identifier &&
				d->name == other.d->name &&
				d->version == other.d->version &&
				d->data == other.d->data);
}

bool UpdateInfo::operator!=(const UpdateInfo &other) const
{
	return d != other.d && (
				d->identifier != other.d->identifier ||
				d->name != other.d->name ||
				d->version != other.d->version ||
				d->data != other.d->data);
}

QVariant UpdateInfo::identifier() const
{
	return d->identifier;
}

QString UpdateInfo::name() const
{
	return d->name;
}

QVersionNumber UpdateInfo::version() const
{
	return d->version;
}

QVariantMap UpdateInfo::data() const
{
	return d->data;
}

void UpdateInfo::setIdentifier(QVariant identifier)
{
	d->identifier = std::move(identifier);
}

void UpdateInfo::setName(QString name)
{
	d->name = std::move(name);
}

void UpdateInfo::setVersion(QVersionNumber version)
{
	d->version = std::move(version);
}

void UpdateInfo::setData(QVariantMap data)
{
	d->data = std::move(data);
}

void UpdateInfo::setData(const QString &key, const QVariant &value)
{
	d->data.insert(key, value);
}

uint QtAutoUpdater::qHash(const UpdateInfo &info, uint seed)
{
	return qHashImpl(info, seed);
}

QDebug QtAutoUpdater::operator<<(QDebug debug, const UpdateInfo &info)
{
	QDebugStateSaver state{debug};
	debug.nospace() << "{Name: " << info.name()
					<< "; Version: " << qUtf8Printable(info.version().toString())
					<< "; Data: " << info.data()
					<< "}";
	return debug;
}
