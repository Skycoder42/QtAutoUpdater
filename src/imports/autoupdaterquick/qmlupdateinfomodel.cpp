#include "qmlupdateinfomodel.h"
using namespace QtAutoUpdater;

QmlUpdateInfoModel::QmlUpdateInfoModel(QObject *parent) :
	QAbstractListModel{parent}
{}

int QmlUpdateInfoModel::rowCount(const QModelIndex &parent) const
{
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return _updateInfos.size();
}

QVariant QmlUpdateInfoModel::data(const QModelIndex &index, int role) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::IndexIsValid));
	switch (role) {
	case NameRole:
		return _updateInfos[index.row()].name();
	case VersionRole:
		return _updateInfos[index.row()].version().toString();
	case SecondaryInfoRole:
		return _updateInfos[index.row()].data().value(_secondaryInfo);
	case IdentifierRole:
		return _updateInfos[index.row()].identifier();
	case GadgetRole:
		return QVariant::fromValue(_updateInfos[index.row()]);
	default:
		return {};
	}
}

QHash<int, QByteArray> QmlUpdateInfoModel::roleNames() const
{
	static const QHash<int, QByteArray> roleNames {
		{NameRole, "name"},
		{VersionRole, "version"},
		{SecondaryInfoRole, "secondaryInfo"},
		{IdentifierRole, "identifier"},
		{GadgetRole, "gadget"}
	};
	return roleNames;
}

void QmlUpdateInfoModel::setUpdateInfos(QList<QtAutoUpdater::UpdateInfo> updateInfos)
{
	if (_updateInfos == updateInfos)
		return;

	beginResetModel();
	_updateInfos = std::move(updateInfos);
	emit updateInfosChanged(_updateInfos);
	endResetModel();
}
