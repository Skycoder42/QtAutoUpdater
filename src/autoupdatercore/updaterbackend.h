#ifndef QTAUTOUPDATER_UPDATERBACKEND_H
#define QTAUTOUPDATER_UPDATERBACKEND_H

#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qversionnumber.h>
#include <QtCore/qvariant.h>

#include "QtAutoUpdaterCore/qtautoupdatercore_global.h"
#include "QtAutoUpdaterCore/adminauthoriser.h"

namespace QtAutoUpdater {

class UpdateInstaller;

struct UpdateInfoPrivate;
struct Q_AUTOUPDATERCORE_EXPORT UpdateInfo
{
	Q_GADGET

	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QVersionNumber version READ version WRITE setVersion)
	Q_PROPERTY(quint64 size READ size WRITE setSize)
	Q_PROPERTY(QVariant identifier READ identifier WRITE setIdentifier)

public:
	UpdateInfo();
	UpdateInfo(UpdateInfoPrivate *d_ptr);
	~UpdateInfo();
	UpdateInfo(const UpdateInfo &other);
	UpdateInfo(UpdateInfo &&other) noexcept;
	UpdateInfo &operator=(const UpdateInfo &other);
	UpdateInfo &operator=(UpdateInfo &&other) noexcept;

	QString name() const;
	QVersionNumber version() const;
	quint64 size() const;
	QVariant identifier() const;

	void setName(QString name);
	void setVersion(QVersionNumber version);
	void setSize(quint64 size);
	void setIdentifier(QVariant identifier);

private:
	QSharedDataPointer<UpdateInfoPrivate> d;
};

class UpdaterBackendPrivate;
class Q_AUTOUPDATERCORE_EXPORT UpdaterBackend : public QObject
{
	Q_OBJECT

public:
	enum class Feature {
		CheckUpdates = 0x00,
		CheckProgress = 0x01,
		TriggerInstall = 0x02,
		InstallNeedsExit = 0x04,
		InstallSelected = 0x08,
		PerformInstall = 0x10
	};
	Q_DECLARE_FLAGS(Features, Feature)
	Q_FLAG(Features)

	explicit UpdaterBackend(QObject *parent = nullptr);

	virtual Features features() const = 0;
	virtual bool initialize(const QVariantMap &arguments,
							AdminAuthoriser *authoriser) = 0;

	virtual UpdateInstaller *installUpdates(const QList<UpdateInfo> &infos) = 0;

public Q_SLOTS:
	virtual void checkForUpdates() = 0;
	virtual void abort(bool force) = 0;

	virtual bool triggerUpdates(const QList<UpdateInfo> &infos) = 0;

Q_SIGNALS:
	void checkDone(const QList<UpdateInfo> &updates);
	void error(const QString &errorMsg);

	void updateProgress(double percent, const QString &status);

protected:
	UpdaterBackend(UpdaterBackendPrivate &dd, QObject *parent);

private:
	Q_DECLARE_PRIVATE(UpdaterBackend)
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtAutoUpdater::UpdaterBackend::Features)

#endif // QTAUTOUPDATER_UPDATERBACKEND_H
