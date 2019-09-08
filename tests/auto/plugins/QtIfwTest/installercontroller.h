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

	bool createRepository();
	bool createInstaller();
	bool installLocal();

	QVersionNumber version() const;
	QString maintenanceToolPath() const;

public Q_SLOTS:
	void setVersion(QVersionNumber version);

private:
	QVersionNumber _version;
	QTemporaryDir _buildDir;

	static QString toSystemExe(QString basePath);
};

#endif // INSTALLERCONTROLLER_H
