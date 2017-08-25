#include "installercontroller.h"

#include <QProcess>
#include <QtTest>

static const QString configFile = QStringLiteral(SRCDIR) + QStringLiteral("../../../installer/config/config.xml");
static const QString configScript = QStringLiteral(SRCDIR) + QStringLiteral("../../../installer/config/controller.qs");
static const QString pkgFile = QStringLiteral(SRCDIR) + QStringLiteral("../../../installer/packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml");
static const QString binarycreator = QStringLiteral(BINDIR) + QStringLiteral("binarycreator");
static const QString repogen = QStringLiteral(BINDIR) + QStringLiteral("repogen");

InstallerController::InstallerController(QObject *parent) :
	QObject(parent),
	_version(1, 0, 0),
	_buildDir()
{
	setVersion(_version);
}

void InstallerController::createRepository()
{
	qDebug() << "Creating repository";
	auto res = QProcess::execute(repogen, {
									 QStringLiteral("--update-new-components"),
									 QStringLiteral("-p"),
									 _buildDir.path() + QStringLiteral("/packages"),
									 _buildDir.path() + QStringLiteral("/repository")});
	QCOMPARE(res, 0);
}

void InstallerController::createInstaller()
{
	qDebug() << "Creating installer";
	QFile configSrc(configFile);
	QFile configOut(_buildDir.path() + QStringLiteral("/config.xml"));

	QVERIFY(configSrc.open(QIODevice::ReadOnly | QIODevice::Text));
	QVERIFY(configOut.open(QIODevice::WriteOnly | QIODevice::Text));
	auto src = QString::fromUtf8(configSrc.readAll());
	configOut.write(src
					.arg(_buildDir.path() + QStringLiteral("/install"))
					.arg(_buildDir.path())
					.toUtf8());
	configSrc.close();
	configOut.close();

	auto res = QProcess::execute(binarycreator, {
									 QStringLiteral("-n"),
									 QStringLiteral("-c"),
									 configOut.fileName(),
									 QStringLiteral("-p"),
									 _buildDir.path() + QStringLiteral("/packages"),
									 _buildDir.path() + QStringLiteral("/QtAutoUpdaterTestInstaller")});
	QCOMPARE(res, 0);
}

QString toSystemExe(QString basePath)
{
#if defined(Q_OS_WIN32)
	if(!basePath.endsWith(QStringLiteral(".exe")))
		return basePath + QStringLiteral(".exe");
	else
		return basePath;
#elif defined(Q_OS_OSX)
	if(basePath.endsWith(QStringLiteral(".app")))
		basePath.truncate(basePath.lastIndexOf(QStringLiteral(".")));
	return basePath + QStringLiteral(".app/Contents/MacOS/") + QFileInfo(basePath).fileName();
#elif defined(Q_OS_UNIX)
	return basePath;
#endif
}

void InstallerController::installLocal()
{
	qDebug() << "Installing example";
	QVERIFY(QFile::exists(toSystemExe(_buildDir.path() + QStringLiteral("/QtAutoUpdaterTestInstaller"))));
#if defined(Q_OS_WIN32)
	auto res = QProcess::execute(toSystemExe(_buildDir.path() + QStringLiteral("/QtAutoUpdaterTestInstaller")),
								 {
												 QStringLiteral("-platform"),
												 QStringLiteral("windows"),
												 QStringLiteral("--script"),
												 configScript
								 });
#else
	auto res = QProcess::execute(toSystemExe(_buildDir.path() + QStringLiteral("/QtAutoUpdaterTestInstaller")),
								 {QStringLiteral("--script"), configScript});
#endif
	QCOMPARE(res, 0);
	QThread::sleep(3);//wait to make shure the asynchronous renaming completed
}

void InstallerController::runUpdater()
{
	qDebug() << "Running updater";
}

QVersionNumber InstallerController::version() const
{
	return _version;
}

QString InstallerController::maintenanceToolPath() const
{
	return _buildDir.path() + QStringLiteral("/install");
}

void InstallerController::setVersion(QVersionNumber version)
{
	qDebug() << "Updating repository version";
	_version = version;

	QDir tDir(_buildDir.path());
	QVERIFY(tDir.mkpath(QStringLiteral("packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta")));
	QVERIFY(tDir.mkpath(QStringLiteral("packages/de.skycoder42.QtAutoUpdaterTestInstaller/data")));

	QFile pkgSrc(pkgFile);
	QFile pkgOut(_buildDir.path() + QStringLiteral("/packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml"));

	QVERIFY(pkgSrc.open(QIODevice::ReadOnly | QIODevice::Text));
	QVERIFY(pkgOut.open(QIODevice::WriteOnly | QIODevice::Text));
	auto src = QString::fromUtf8(pkgSrc.readAll());
	pkgOut.write(src.arg(_version.toString()).toUtf8());
	pkgSrc.close();
	pkgOut.close();

	QFile dataFile(_buildDir.path() + QStringLiteral("/packages/de.skycoder42.QtAutoUpdaterTestInstaller/data/version.txt"));
	QVERIFY(dataFile.open(QIODevice::WriteOnly | QIODevice::Text));
	dataFile.write(_version.toString().toUtf8());
	dataFile.close();
}
