#ifndef QTAUTOUPDATER_UPDATEINFO_H
#define QTAUTOUPDATER_UPDATEINFO_H

#include <QtCore/qshareddata.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"

namespace QtAutoUpdater {

struct UpdateInfoPrivate;
//! Provides information about updates for components
struct Q_AUTOUPDATERCORE_EXPORT UpdateInfo
{
	Q_GADGET

	//! An unspecified identifier for the update
	Q_PROPERTY(QVariant identifier READ identifier WRITE setIdentifier)
	//! The name of the component that has an update
	Q_PROPERTY(QString name READ name WRITE setName)
	//! The new version for that compontent
	Q_PROPERTY(QVersionNumber version READ version WRITE setVersion)
	//! Additional properties of the update
	Q_PROPERTY(QVariantMap data READ data WRITE setData)

public:
	//! Default constructor
	UpdateInfo();
	//! Initializing constructor
	UpdateInfo(QVariant identifier,
			   QString name,
			   QVersionNumber version,
			   QVariantMap data = {});
	//! @private
	UpdateInfo(UpdateInfoPrivate *d_ptr);
	~UpdateInfo();
	//! Copy constructor
	UpdateInfo(const UpdateInfo &other);
	//! Move constructor
	UpdateInfo(UpdateInfo &&other) noexcept;
	//! Copy assignment operator
	UpdateInfo &operator=(const UpdateInfo &other);
	//! Move assignment operator
	UpdateInfo &operator=(UpdateInfo &&other) noexcept;

	//! Equality operator
	bool operator==(const UpdateInfo &other) const;
	//! Inequality operator
	bool operator!=(const UpdateInfo &other) const;

	//! @readAcFn{UpdateInfo::identifier}
	QVariant identifier() const;
	//! @readAcFn{UpdateInfo::name}
	QString name() const;
	//! @readAcFn{UpdateInfo::version}
	QVersionNumber version() const;
	//! @readAcFn{UpdateInfo::data}
	QVariantMap data() const;

	//! @writeAcFn{UpdateInfo::identifier}
	void setIdentifier(QVariant identifier);
	//! @writeAcFn{UpdateInfo::name}
	void setName(QString name);
	//! @writeAcFn{UpdateInfo::version}
	void setVersion(QVersionNumber version);
	//! @writeAcFn{UpdateInfo::data}
	void setData(QVariantMap data);
	//! @writeAcFn{UpdateInfo::data}
	void setData(const QString &key, const QVariant &value);

private:
	QSharedDataPointer<UpdateInfoPrivate> d;
};

//! QHash hashkey operator
Q_AUTOUPDATERCORE_EXPORT uint qHash(const UpdateInfo &info, uint seed = 0);
//! QDebug stream operator
Q_AUTOUPDATERCORE_EXPORT QDebug operator<<(QDebug debug, const UpdateInfo &info);

}

Q_DECLARE_METATYPE(QtAutoUpdater::UpdateInfo)
Q_DECLARE_TYPEINFO(QtAutoUpdater::UpdateInfo, Q_MOVABLE_TYPE);

#endif // QTAUTOUPDATER_UPDATEINFO_H
