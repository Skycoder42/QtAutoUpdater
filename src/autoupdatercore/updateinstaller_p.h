#ifndef QTAUTOUPDATER_UPDATEINSTALLER_P_H
#define QTAUTOUPDATER_UPDATEINSTALLER_P_H

#include "updateinstaller.h"

#include <optional>

#include <QtCore/QMap>
#include <QtCore/QLoggingCategory>

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater {

class Q_AUTOUPDATERCORE_EXPORT UpdateInstallerPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdateInstaller)

public:
	using ComponentInfo = std::pair<UpdateInfo, bool>;

	QMap<QVariant, ComponentInfo> components;
	mutable std::optional<QList<UpdateInfo>> componentList = std::nullopt;
	ComponentModel *componentModel = nullptr;
};

class Q_AUTOUPDATERCORE_EXPORT ComponentModel : public QAbstractListModel
{
	Q_OBJECT

public:
	ComponentModel(UpdateInstaller *parent);

	void reset(QList<UpdateInstallerPrivate::ComponentInfo> data);

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
	UpdateInstaller *_installer;
	QList<UpdateInstallerPrivate::ComponentInfo> _data;
};

Q_DECLARE_LOGGING_CATEGORY(logInstaller)

}

#endif // QTAUTOUPDATER_UPDATEINSTALLER_P_H
