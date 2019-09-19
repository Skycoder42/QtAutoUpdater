#ifndef QMLUPDATEINFOMODEL_H
#define QMLUPDATEINFOMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtAutoUpdaterCore/UpdateInfo>

class QmlUpdateInfoModel : public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY(QString secondaryInfo MEMBER _secondaryInfo NOTIFY secondaryInfoChanged)
	Q_PROPERTY(QList<QtAutoUpdater::UpdateInfo> updateInfos MEMBER _updateInfos WRITE setUpdateInfos NOTIFY updateInfosChanged)

public:
	enum Roles {
		NameRole = Qt::UserRole,
		VersionRole,
		SecondaryInfoRole,
		IdentifierRole,
		GadgetRole
	};
	Q_ENUM(Roles)

	explicit QmlUpdateInfoModel(QObject *parent = nullptr);

	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	void secondaryInfoChanged(const QString &secondaryInfos);
	void updateInfosChanged(const QList<QtAutoUpdater::UpdateInfo> &updateInfos);

private:
	QString _secondaryInfo;
	QList<QtAutoUpdater::UpdateInfo> _updateInfos;

	void setUpdateInfos(QList<QtAutoUpdater::UpdateInfo> updateInfos);
};

#endif // QMLUPDATEINFOMODEL_H
