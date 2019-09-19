#ifndef QTAUTOUPDATER_UPDATEINFO_H
#define QTAUTOUPDATER_UPDATEINFO_H

#include <QtCore/qshareddata.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"

namespace QtAutoUpdater {

struct UpdateInfoPrivate;
struct Q_AUTOUPDATERCORE_EXPORT UpdateInfo
{
	Q_GADGET

	Q_PROPERTY(QVariant identifier READ identifier WRITE setIdentifier)
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QVersionNumber version READ version WRITE setVersion)
	Q_PROPERTY(QVariantMap data READ data WRITE setData)

public:
	UpdateInfo();
	UpdateInfo(QVariant identifier,
			   QString name,
			   QVersionNumber version,
			   QVariantMap data = {});
	UpdateInfo(UpdateInfoPrivate *d_ptr);
	~UpdateInfo();
	UpdateInfo(const UpdateInfo &other);
	UpdateInfo(UpdateInfo &&other) noexcept;
	UpdateInfo &operator=(const UpdateInfo &other);
	UpdateInfo &operator=(UpdateInfo &&other) noexcept;

	bool operator==(const UpdateInfo &other) const;
	bool operator!=(const UpdateInfo &other) const;

	QVariant identifier() const;
	QString name() const;
	QVersionNumber version() const;
	QVariantMap data() const;

	void setIdentifier(QVariant identifier);
	void setName(QString name);
	void setVersion(QVersionNumber version);
	void setData(QVariantMap data);
	void setData(const QString &key, const QVariant &value);

private:
	QSharedDataPointer<UpdateInfoPrivate> d;
};

Q_AUTOUPDATERCORE_EXPORT uint qHash(const UpdateInfo &info, uint seed = 0);
Q_AUTOUPDATERCORE_EXPORT QDebug operator<<(QDebug debug, const UpdateInfo &info);

}

Q_DECLARE_METATYPE(QtAutoUpdater::UpdateInfo)
Q_DECLARE_TYPEINFO(QtAutoUpdater::UpdateInfo, Q_MOVABLE_TYPE);

#endif // QTAUTOUPDATER_UPDATEINFO_H
