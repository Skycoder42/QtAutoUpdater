#ifndef INSTALLERCONTROLLER_H
#define INSTALLERCONTROLLER_H

#include <QObject>
#include <QVersionNumber>
#include <QTemporaryDir>

class InstallerController : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QVersionNumber version READ version WRITE setVersion)

public:
	explicit InstallerController(QObject *parent = nullptr);

	void createRepository();
	void createInstaller();
	void installLocal();
	void runUpdater();

	QVersionNumber version() const;
	QString maintenanceToolPath() const;

public slots:
	void setVersion(QVersionNumber version);

private:
	QVersionNumber _version;
	QTemporaryDir _buildDir;
};

#endif // INSTALLERCONTROLLER_H
