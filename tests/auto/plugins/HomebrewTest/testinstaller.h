#ifndef TESTINSTALLER_H
#define TESTINSTALLER_H

#include <QObject>
#include <QVersionNumber>
#include <QTemporaryDir>

class TestInstaller : public QObject
{
	Q_OBJECT

public:
	explicit TestInstaller(QObject *parent = nullptr);
	~TestInstaller() override;

	bool isInstalled() const;

	bool setup();
	bool cleanup();

	void setVersion(QVersionNumber version);
	bool package();
	bool install();
	bool uninstall();

private:
	static const QString FormulaTemplate;

	QTemporaryDir _dir;
	bool _installed = false;
	QVersionNumber _version {1, 0, 0};
	int _changeCounter = 0;

	bool runGit(const QStringList &arguments);
	bool runBrew(const QStringList &arguments, bool verify = true);
	bool runTool(const QString &program, const QStringList &arguments, const QString &workingDir, bool verify);
};

#endif // TESTINSTALLER_H
