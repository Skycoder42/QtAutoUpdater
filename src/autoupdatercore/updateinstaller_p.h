#ifndef QTAUTOUPDATER_UPDATEINSTALLER_P_H
#define QTAUTOUPDATER_UPDATEINSTALLER_P_H

#include "updateinstaller.h"

#include <optional>
#include <tuple>

#include <QtCore/QMap>
#include <QtCore/QLoggingCategory>

#include <QtCore/private/qobject_p.h>

namespace QtAutoUpdater {

class ComponentModel;
class ProgressModel;

class Q_AUTOUPDATERCORE_EXPORT UpdateInstallerPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(UpdateInstaller)

public:
	using ComponentInfo = std::pair<UpdateInfo, bool>;

	QMap<QVariant, ComponentInfo> components;
	mutable std::optional<QList<UpdateInfo>> componentList = std::nullopt;
	ComponentModel *componentModel = nullptr;
	ProgressModel *progressModel = nullptr;
};

class Q_AUTOUPDATERCORE_EXPORT ComponentModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum Roles {
		NameRole = Qt::UserRole,
		VersionRole,
		SelectedRole,
		UpdateInfoRole,

		HeaderSizeHint = Qt::UserRole + 100
	};
	Q_ENUM(Roles)

	ComponentModel(UpdateInstaller *parent);

	void reset(QList<UpdateInstallerPrivate::ComponentInfo> data);

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QHash<int, QByteArray> roleNames() const override;

private:
	UpdateInstaller *_installer;
	QList<UpdateInstallerPrivate::ComponentInfo> _data;
};

class Q_AUTOUPDATERCORE_EXPORT ProgressModel : public QAbstractTableModel
{
	Q_OBJECT

	Q_PROPERTY(int progressColumn READ progressColumn CONSTANT)

public:
	enum Roles {
		NameRole = Qt::UserRole,
		ProgressRole,
		StatusRole,
		UpdateInfoRole,

		HeaderSizeHint = Qt::UserRole + 100
	};
	Q_ENUM(Roles)

	ProgressModel(UpdateInstaller *parent);

	void reset(const QList<UpdateInfo> &data);

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	int progressColumn() const;

private Q_SLOTS:
	void updateComponentProgress(const QVariant &id, double percentage, const QString &status);

private:
	using UpdateState = std::tuple<UpdateInfo, double, QString>;

	UpdateInstaller *_installer;
	QList<UpdateState> _data;
};

Q_DECLARE_LOGGING_CATEGORY(logInstaller)

}

#endif // QTAUTOUPDATER_UPDATEINSTALLER_P_H
