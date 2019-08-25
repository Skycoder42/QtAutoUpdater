#ifndef QTAUTOUPDATER_UPDATEINSTALLER_H
#define QTAUTOUPDATER_UPDATEINSTALLER_H

#include <QtCore/qobject.h>
#include <QtCore/quuid.h>
#include <QtCore/qabstractitemmodel.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/updateinfo.h"

namespace QtAutoUpdater {

class ComponentModel;

class UpdateInstallerPrivate;
class Q_AUTOUPDATERCORE_EXPORT UpdateInstaller : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QList<UpdateInfo> components READ components WRITE setComponents NOTIFY componentsChanged)

public:
	enum class Feature {
		None = 0x00,
		SelectComponents = 0x01,
		DetailedProgress = 0x02,
		CanCancel = 0x04
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	virtual Features features() const = 0;

	virtual QList<UpdateInfo> components() const;

	Q_INVOKABLE virtual QAbstractItemModel *componentModel() const;
	Q_INVOKABLE virtual QAbstractItemModel *progressModel() const;

public Q_SLOTS:
	virtual void startInstall();
	virtual void cancelInstall();
	virtual void eulaHandled(QUuid id, bool accepted) = 0;

	virtual void setComponents(QList<QtAutoUpdater::UpdateInfo> components);

Q_SIGNALS:
	void updateGlobalProgress(double percentage, const QString &status = {});
	void updateComponentProgress(const QVariant &id, double percentage, const QString &status = {});
	void showEula(QUuid id, const QString &htmlText, bool required);
	void installSucceeded(bool shouldRestart);
	void installFailed(const QString &errorMessage);

	void componentsChanged();

protected:
	explicit UpdateInstaller(QObject *parent = nullptr);
	explicit UpdateInstaller(UpdateInstallerPrivate &dd, QObject *parent = nullptr);

	virtual void startInstallImpl() = 0;

	virtual void setComponentEnabled(const QVariant &id, bool enabled);

private:
	Q_DECLARE_PRIVATE(UpdateInstaller)
	Q_DISABLE_COPY(UpdateInstaller)
	friend class QtAutoUpdater::ComponentModel;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdateInstaller::Features)
Q_DECLARE_METATYPE(QtAutoUpdater::UpdateInstaller*)

#endif // QTAUTOUPDATER_UPDATEINSTALLER_H
