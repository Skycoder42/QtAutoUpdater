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
	d->progressModel = new ProgressModel{this};

	connect(this, &UpdateInstaller::installSucceeded,
			this, std::bind(&UpdateInstaller::updateGlobalProgress, this, 1.0, tr("Installation complete")));
	connect(this, &UpdateInstaller::installFailed,
			this, std::bind(&UpdateInstaller::updateGlobalProgress, this, 1.0, tr("Installation failed!")));
}

QList<UpdateInfo> UpdateInstaller::components() const
{
	Q_D(const UpdateInstaller);
	if (!d->componentList) {
		d->componentList = QList<UpdateInfo>{};
		d->componentList->reserve(d->components.size());
		for (const auto &info : d->components) {
			if (info.second)
				d->componentList->append(info.first);
		}
	}
	return *d->componentList;
}

QAbstractItemModel *UpdateInstaller::componentModel() const
{
	Q_D(const UpdateInstaller);
	return d->componentModel;
}

QAbstractItemModel *UpdateInstaller::progressModel() const
{
	Q_D(const UpdateInstaller);
	return d->progressModel;
}

void UpdateInstaller::startInstall()
{
	Q_D(UpdateInstaller);
	d->progressModel->reset(components());
	startInstallImpl();
}

void UpdateInstaller::cancelInstall() {}

void UpdateInstaller::restartApplication()
{
	qCWarning(logInstaller) << "Restarting is not supported by the current backend";
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
	emit componentsChanged();

	// reset the model
	d->componentModel->reset(d->components.values());
}

void UpdateInstaller::setComponentEnabled(const QVariant &id, bool enabled)
{
	Q_D(UpdateInstaller);
	d->components[id].second = enabled;
	d->componentList = std::nullopt;
	emit componentsChanged();
}

// ------------- private implementation -------------

ComponentModel::ComponentModel(UpdateInstaller *parent) :
	QAbstractTableModel{parent},
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

int ComponentModel::columnCount(const QModelIndex &parent) const
{
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return 2;
}

QVariant ComponentModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

	switch (index.column()) {
	case 0:
		switch (role) {
		// normal roles
		case Qt::CheckStateRole:
			if (_installer->features().testFlag(UpdateInstaller::Feature::SelectComponents))
				return _data[index.row()].second ? Qt::Checked : Qt::Unchecked;
			else
				return {};
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		// special roles
		case NameRole:
			return _data[index.row()].first.name();
		case VersionRole:
			return _data[index.row()].first.version().toString();
		case SelectedRole:
			if (_installer->features().testFlag(UpdateInstaller::Feature::SelectComponents))
				return _data[index.row()].second;
			else
				return {};
		case UpdateInfoRole:
			return QVariant::fromValue(_data[index.row()].first);
		default:
			return {};
		}
	case 1:
		switch (role) {
		// normal roles
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
			return _data[index.row()].first.version().toString();
		default:
			return {};
		}
	default:
		return {};
	}
}

bool ComponentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

	if (_installer->features().testFlag(UpdateInstaller::Feature::SelectComponents)) {
		auto &info = _data[index.row()];

		if (index.column() == 0 && role == Qt::CheckStateRole)
			info.second = value.toInt() != Qt::Unchecked;
		else if (index.column() == 0 && role == SelectedRole)
			info.second = value.toBool();
		else
			return false;

		emit dataChanged(index, index, {Qt::CheckStateRole, SelectedRole});
		_installer->setComponentEnabled(info.first.identifier(), info.second);
		return true;
	} else
		return false;
}

QVariant ComponentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
		return {};

	switch (role) {
	case Qt::DisplayRole:
		switch (section) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Version");
		default:
			return {};
		}
	case HeaderSizeHint:
		switch (section) {
		case 0:
			return -1;  // -QHeaderView::Stretch
		case 1:
			return -3;  // -QHeaderView::ResizeToContents
		default:
			return {};
		}
	default:
		return {};
	}
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

QHash<int, QByteArray> ComponentModel::roleNames() const
{
	return {
		{NameRole, "name"},
		{VersionRole, "version"},
		{SelectedRole, "selected"},
		{UpdateInfoRole, "updateInfo"}
	};
}



ProgressModel::ProgressModel(UpdateInstaller *parent) :
	QAbstractTableModel{parent},
	_installer{parent}
{
	connect(_installer, &UpdateInstaller::updateComponentProgress,
			this, &ProgressModel::updateComponentProgress);
}

void ProgressModel::reset(const QList<UpdateInfo> &data)
{
	beginResetModel();
	_data.clear();
	for (const auto &info : data)
		_data.append({info, 0.0, {}});
	endResetModel();
}

int ProgressModel::rowCount(const QModelIndex &parent) const
{
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return _data.size();
}

int ProgressModel::columnCount(const QModelIndex &parent) const
{
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return 3;
}

QVariant ProgressModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid));

	switch (index.column()) {
	case 0:
		switch (role) {
		// normal roles
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
		// special roles
		case NameRole:
			return std::get<0>(_data[index.row()]).name();
		case ProgressRole:
			return std::get<1>(_data[index.row()]);
		case StatusRole:
			return std::get<2>(_data[index.row()]);
		case UpdateInfoRole:
			return QVariant::fromValue(std::get<0>(_data[index.row()]));
		default:
			return {};
		}
	case 1:
		switch (role) {
		// normal roles
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
			return std::get<2>(_data[index.row()]);
		default:
			return {};
		}
	case 2:
		switch (role) {
		// normal roles
		case Qt::DisplayRole:
		case Qt::ToolTipRole:
			return std::get<1>(_data[index.row()]);
		default:
			return {};
		}
	default:
		return {};
	}
}

QVariant ProgressModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
		return {};

	switch (role) {
	case Qt::DisplayRole:
		switch (section) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Status");
		case 2:
			return tr("Progress");
		default:
			return {};
		}
	case HeaderSizeHint:
		switch (section) {
		case 0:
			return -3;  // -QHeaderView::ResizeToContents
		case 1:
			return -1;  // -QHeaderView::Stretch
		case 2:
			return 120;
		default:
			return {};
		}
	default:
		return {};
	}
}

QHash<int, QByteArray> ProgressModel::roleNames() const
{
	return {
		{NameRole, "name"},
		{ProgressRole, "progress"},
		{StatusRole, "status"},
		{UpdateInfoRole, "updateInfo"}
	};
}

int ProgressModel::progressColumn() const
{
	return 2;
}

void ProgressModel::updateComponentProgress(const QVariant &id, double percentage, const QString &status)
{
	const auto compIt = std::find_if(_data.begin(), _data.end(), [id](const UpdateState &state) {
		return std::get<0>(state).identifier() == id;
	});
	if (compIt == _data.end())
		return;
	std::get<1>(*compIt) = percentage;
	std::get<2>(*compIt) = status;
	const auto compIdx = compIt - _data.begin();
	emit dataChanged(index(compIdx, 0),
					 index(compIdx, 2),
					 {Qt::DisplayRole, Qt::ToolTipRole, ProgressRole, StatusRole});
}

#include "moc_updateinstaller.cpp"
