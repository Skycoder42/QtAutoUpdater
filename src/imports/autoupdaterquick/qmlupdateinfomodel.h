#ifndef QMLUPDATEINFOMODEL_H
#define QMLUPDATEINFOMODEL_H

#include <QtCore/QAbstractListModel>

#include <QtAutoUpdaterCore/UpdateInfo>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtAutoUpdater::Quick {

/*! @brief An item model that takes a list of update infos and presents them via the model
 *
 * @since 3.0
 */
class UpdateInfoModel : public QAbstractListModel
#else
namespace QtAutoUpdater {

class QmlUpdateInfoModel : public QAbstractListModel
#endif
{
	Q_OBJECT

	/*! @brief The secondary info property name
	 *
	 * @default{_&lt;empty&gt;_}
	 *
	 * The secondaryInfo as defined by QtAutoUpdater::UpdaterBackend::secondaryInfo. This should be
	 * the `first` as returned from that method. If set, the value of that property within data is
	 * provided via the secondaryInfo role.
	 *
	 * @accessors{
	 *	@memberAc{secondaryInfo}
	 * }
	 *
	 * @sa QtAutoUpdater::UpdaterBackend::secondaryInfo
	 */
	Q_PROPERTY(QString secondaryInfo MEMBER _secondaryInfo NOTIFY secondaryInfoChanged)
	/*! @brief The update infos to be provided by the model
	 *
	 * @default{_&lt;empty&gt;_}
	 *
	 * The infos are simply presented as list item model, with the name, version, identifier and
	 * secondaryInfo properties provided as equally named item roles. In addition, there is the
	 * gadget role, which allows access to the whole update info gadget.
	 *
	 * @accessors{
	 *	@memberAc{updateInfos}
	 * }
	 *
	 * @sa UpdateInfoModel::secondaryInfo, QtAutoUpdater::UpdateInfo
	 */
	Q_PROPERTY(QList<QtAutoUpdater::UpdateInfo> updateInfos MEMBER _updateInfos WRITE setUpdateInfos NOTIFY updateInfosChanged)

public:
	//! The roles that the item model provides
	enum Roles {
		NameRole = Qt::UserRole,  //!< Provided as "name", the QtAutoUpdater::UpdateInfo::name property
		VersionRole,  //!< Provided as "version", the QtAutoUpdater::UpdateInfo::version property
		SecondaryInfoRole,  //!< Provided as "secondaryInfo", the secondaryInfo value of the QtAutoUpdater::UpdateInfo::data property, as declared by UpdateInfoModel::secondaryInfo
		IdentifierRole,  //!< Provided as "identifier", the QtAutoUpdater::UpdateInfo::identifier property
		GadgetRole  //!< Provided as "gadget", the whole QtAutoUpdater::UpdateInfo gadget
	};
	Q_ENUM(Roles)

	//! @private
	explicit QmlUpdateInfoModel(QObject *parent = nullptr);

	//! @private
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @private
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	//! @private
	QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
	//! @private
	void secondaryInfoChanged(const QString &secondaryInfos);
	//! @private
	void updateInfosChanged(const QList<QtAutoUpdater::UpdateInfo> &updateInfos);

private:
	QString _secondaryInfo;
	QList<QtAutoUpdater::UpdateInfo> _updateInfos;

	void setUpdateInfos(QList<QtAutoUpdater::UpdateInfo> updateInfos);
};

}

#endif // QMLUPDATEINFOMODEL_H
