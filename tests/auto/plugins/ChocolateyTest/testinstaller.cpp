#include "testinstaller.h"
#include <plugintest.h>

const QByteArray TestInstaller::XmlText { R"_(<?xml version="1.0" encoding="utf-8"?>
<package xmlns="http://schemas.microsoft.com/packaging/2015/06/nuspec.xsd">
	<metadata>
		<id>qtautoupdater-test-package</id>
		<version>1.0.0</version>
		<title>qtautoupdater-test-package</title>
		<authors>Skycoder42</authors>
		<projectUrl>https://github.com/Skycoder42/QtAutoUpdater</projectUrl>
		<tags>test-package</tags>
		<summary>A test package for testing the chocolatey plugin of QtAutoUpdater</summary>
		<description>This package does nothing and has no functionality.</description>
	</metadata>
	<files>
		<file src="tools\**" target="tools" />
	</files>
</package>
)_"};

TestInstaller::TestInstaller(QObject *parent) :
	QObject{parent}
{}

TestInstaller::~TestInstaller()
{
	if (_installed)
		uninstall();
}

QString TestInstaller::repositoryPath() const
{
	return QDir::toNativeSeparators(_dir.filePath(QStringLiteral("repository")));
}

bool TestInstaller::isInstalled() const
{
	return _installed;
}

bool TestInstaller::setup()
{
	TEST_WRAP_BEGIN

	QVERIFY(QDir{_dir.path()}.mkpath(QStringLiteral("repository")));
	QVERIFY(QDir{_dir.path()}.mkpath(QStringLiteral("packages/qtautoupdater-test-package/tools")));

	for (const auto &fName : {QStringLiteral("chocolateyinstall"), QStringLiteral("chocolateyuninstall")}) {
		QFile file{_dir.filePath(QStringLiteral("packages/qtautoupdater-test-package/tools/%1.ps1").arg(fName))};
		QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
		file.write("$ErrorActionPreference = 'Stop';\n");
		file.close();
	}

	QFile xmlFile{_dir.filePath(QStringLiteral("packages/qtautoupdater-test-package/qtautoupdater-test-package.nuspec"))};
	QVERIFY(xmlFile.open(QIODevice::WriteOnly | QIODevice::Text));
	xmlFile.write(XmlText);
	xmlFile.close();

	QVERIFY(uninstall());

	TEST_WRAP_END
}

void TestInstaller::setVersion(QVersionNumber version)
{
	_version = std::move(version);
}

bool TestInstaller::package()
{
	TEST_WRAP_BEGIN

	QDir repoDir {_dir.filePath(QStringLiteral("repository"))};
	QVERIFY(repoDir.removeRecursively());
	QVERIFY(repoDir.mkpath(QStringLiteral(".")));
	QVERIFY(runChoco({
						 QStringLiteral("pack"),
						 QDir::toNativeSeparators(_dir.filePath(QStringLiteral("packages/qtautoupdater-test-package/qtautoupdater-test-package.nuspec"))),
						 QStringLiteral("--version"),
						 _version.toString(),
						 QStringLiteral("--out"),
						 QDir::toNativeSeparators(repoDir.absolutePath())
					 }, false));

	TEST_WRAP_END
}

bool TestInstaller::install()
{
	TEST_WRAP_BEGIN

	QVERIFY2(!_installed, "You must run uninstall before installing again");
	QVERIFY(runChoco({
						 QStringLiteral("install"),
						 QStringLiteral("qtautoupdater-test-package")
					 }, true));
	_installed = true;

	TEST_WRAP_END
}

bool TestInstaller::uninstall()
{
	TEST_WRAP_BEGIN

	QVERIFY(runChoco({
						 QStringLiteral("uninstall"),
						 QStringLiteral("qtautoupdater-test-package")
					 }, false, _installed));
	_installed = false;

	TEST_WRAP_END
}

bool TestInstaller::runChoco(QStringList arguments, bool source, bool verify)
{
	TEST_WRAP_BEGIN

	const auto chocoExe = QStandardPaths::findExecutable(QStringLiteral("choco"));
	QVERIFY(!chocoExe.isEmpty());

	if (source) {
		arguments.append({
							 QStringLiteral("-s"),
							 _dir.filePath(QStringLiteral("repository"))
						 });
	}

	QProcess chocoProc;
	chocoProc.setProgram(chocoExe);
	chocoProc.setArguments(arguments);
	chocoProc.setProcessChannelMode(QProcess::ForwardedChannels);
	chocoProc.setStandardInputFile(QProcess::nullDevice());

	qDebug() << "Executing" << chocoExe << arguments;
	chocoProc.start(QIODevice::ReadOnly);
	const auto chocoProcOk = chocoProc.waitForFinished();
	QVERIFY2(chocoProcOk, qUtf8Printable(chocoProc.errorString()));
	if (verify)
		QCOMPARE(chocoProc.exitCode(), EXIT_SUCCESS);
	else if (chocoProc.exitCode() != EXIT_SUCCESS)
		qWarning() << "Choco process exited with unclean exit code:" << chocoProc.exitCode();

	TEST_WRAP_END
}
