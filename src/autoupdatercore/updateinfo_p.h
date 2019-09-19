#ifndef QTAUTOUPDATER_UPDATEINFO_P_H
#define QTAUTOUPDATER_UPDATEINFO_P_H

#include "updateinfo.h"

namespace QtAutoUpdater {

struct UpdateInfoPrivate : public QSharedData
{
	QVariant identifier;
	QString name;
	QVersionNumber version;
	QVariantMap data;
};

}

#endif // QTAUTOUPDATER_UPDATEINFO_P_H
