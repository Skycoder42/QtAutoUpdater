#include "installercontroller.h"

#include <QProcess>
#include <QtTest>

static const QString configFile = SRCDIR + QStringLiteral("../../../installer/config/config.xml");
static const QString configScript = SRCDIR + QStringLiteral("../../../installer/config/controller.qs");
static const QString pkgFile = SRCDIR + QStringLiteral("../../../installer/packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml");
static const QString binarycreator = BINDIR + QStringLiteral("binarycreator");
static const QString repogen = BINDIR + QStringLiteral("repogen");

InstallerController::InstallerController(QObject *parent) :
	QObject(parent),
	_version(1, 0, 0),
	_buildDir()
{
	setVersion(_version);
}

void InstallerController::createRepository()
{
	auto res = QProcess::execute(repogen, {"--update-new-components", "-p", _buildDir.path() + "/packages", _buildDir.path() + "/repository"});
	QCOMPARE(res, 0);
}

void InstallerController::createInstaller()
{
	QFile configSrc(configFile);
	QFile configOut(_buildDir.path() + "/config.xml");

	QVERIFY(configSrc.open(QIODevice::ReadOnly | QIODevice::Text));
	QVERIFY(configOut.open(QIODevice::WriteOnly | QIODevice::Text));
	auto src = QString::fromUtf8(configSrc.readAll());
	configOut.write(src
					.arg(_buildDir.path() + "/install")
					.arg(_buildDir.path())
					.toUtf8());
	configSrc.close();
	configOut.close();

	auto res = QProcess::execute(binarycreator, {"-n", "-c", configOut.fileName(), "-p", _buildDir.path() + "/packages", _buildDir.path() + "/QtAutoUpdaterTestInstaller"});
	QCOMPARE(res, 0);
}

void InstallerController::installLocal()
{
	auto res = QProcess::execute(_buildDir.path() + "/QtAutoUpdaterTestInstaller", {"--script", configScript});
	QCOMPARE(res, 0);
}

void InstallerController::runUpdater()
{

}

QVersionNumber InstallerController::version() const
{
	return _version;
}

QString InstallerController::maintenanceToolPath() const
{
	return _buildDir.path() + "/install";
}

void InstallerController::setVersion(QVersionNumber version)
{
	_version = version;

	QDir tDir(_buildDir.path());
	QVERIFY(tDir.mkpath("packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta"));
	QVERIFY(tDir.mkpath("packages/de.skycoder42.QtAutoUpdaterTestInstaller/data"));

	QFile pkgSrc(pkgFile);
	QFile pkgOut(_buildDir.path() + "/packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml");

	QVERIFY(pkgSrc.open(QIODevice::ReadOnly | QIODevice::Text));
	QVERIFY(pkgOut.open(QIODevice::WriteOnly | QIODevice::Text));
	auto src = QString::fromUtf8(pkgSrc.readAll());
	pkgOut.write(src.arg(_version.toString()).toUtf8());
	pkgSrc.close();
	pkgOut.close();

	QFile dataFile(_buildDir.path() + "/packages/de.skycoder42.QtAutoUpdaterTestInstaller/data/version.txt");
	QVERIFY(dataFile.open(QIODevice::WriteOnly | QIODevice::Text));
	dataFile.write(_version.toString().toUtf8());
	dataFile.close();
}
