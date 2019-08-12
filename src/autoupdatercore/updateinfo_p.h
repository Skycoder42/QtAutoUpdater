#ifndef QTAUTOUPDATER_UPDATEINFO_P_H
#define QTAUTOUPDATER_UPDATEINFO_P_H

#include "updateinfo.h"

namespace QtAutoUpdater {

struct UpdateInfoPrivate : public QSharedData
{
	QString name;
	QVersionNumber version;
	quint64 size = 0;
	QVariant identifier;
};

}

#endif // QTAUTOUPDATER_UPDATEINFO_P_H
