#include "updateinstaller.h"
#include "updateinstaller_p.h"
using namespace QtAutoUpdater;

namespace QtAutoUpdater {

Q_LOGGING_CATEGORY(logInstaller, "qt.autoupdater.core.UpdateInstaller")

}

UpdateInstaller::UpdateInstaller(QObject *parent) :
	UpdateInstaller{*new UpdateInstallerPrivate{}, parent}
{}

UpdateInstaller::UpdateInstaller(UpdateInstallerPrivate &dd, QObject *parent) :
	QObject{dd, parent}
{
	Q_D(UpdateInstaller);
	d->componentModel = new ComponentModel{this};
}

QList<UpdateInfo> UpdateInstaller::components() const
{
	const Q_D(UpdateInstaller);
	if (!d->componentList) {
		d->componentList = QList<UpdateInfo>{};
		d->componentList->reserve(d->components.size());
		for (const auto &info : d->components)
			d->componentList->append(info.first);
	}
	return *d->componentList;
}

QAbstractListModel *UpdateInstaller::componentModel() const
{
	const Q_D(UpdateInstaller);
	return d->componentModel;
}

QAbstractListModel *UpdateInstaller::progressModel() const
{
	Q_UNIMPLEMENTED();
	return nullptr;
}

void UpdateInstaller::setComponents(QList<UpdateInfo> components)
{
	Q_D(UpdateInstaller);
	d->components.clear();
	d->componentList = std::nullopt;
	for (auto &&info : std::move(components)) {
		const auto id = info.identifier();
		if (id.isValid())
			d->components.insert(id, {std::move(info), true});
		else
			qCWarning(logInstaller) << "Cannot install component" << info.name() << "without a valid identifier!";
	}
	emit componentsChanged(*d->componentList);

	// update the model
	d->componentModel->reset(d->components.values());
}

void UpdateInstaller::setComponentEnabled(const QVariant &id, bool enabled)
{
	Q_D(UpdateInstaller);
	d->components[id].second = enabled;
}

// ------------- private implementation -------------

ComponentModel::ComponentModel(UpdateInstaller *parent) :
	QAbstractListModel{parent},
	_installer{parent}
{}

void ComponentModel::reset(QList<UpdateInstallerPrivate::ComponentInfo> data)
{
	beginResetModel();
	_data = std::move(data);
	endResetModel();
}

int ComponentModel::rowCount(const QModelIndex &parent) const
{
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return _data.size();
}

QVariant ComponentModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

	switch (role) {
	case Qt::DisplayRole:
		return _data[index.row()].first.name();
	case Qt::ToolTipRole:
		return tr("Newer version: %1 – Update size: %L2")
				.arg(_data[index.row()].first.version().toString())
				.arg(_data[index.row()].first.size());
	case Qt::CheckStateRole:
		return _data[index.row()].second ? Qt::Checked : Qt::Unchecked;
	default:
		return {};
	}
}

bool ComponentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

	if (role == Qt::CheckStateRole) {
		auto &info = _data[index.row()];
		info.second = value.toInt() != Qt::Unchecked;
		emit dataChanged(index, index, {role});
		_installer->setComponentEnabled(info.first.identifier(), info.second);
		return true;
	} else
		return false;
}

QVariant ComponentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return tr("Update Component");
	else
		return {};
}

Qt::ItemFlags ComponentModel::flags(const QModelIndex &index) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::DoNotUseParent));
	if (index.isValid()) {
		return Qt::ItemIsSelectable |
				Qt::ItemIsUserCheckable |
				Qt::ItemIsEnabled |
				Qt::ItemNeverHasChildren;
	} else
		return Qt::NoItemFlags;
}

#include "moc_updateinstaller.cpp"
