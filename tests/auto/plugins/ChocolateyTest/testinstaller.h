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
	~TestInstaller();

	QString repositoryPath() const;
	bool isInstalled() const;

	bool setup();

	void setVersion(QVersionNumber version);
	bool package();
	bool install();
	bool uninstall();

private:
	static const QByteArray XmlText;

	QTemporaryDir _dir;
	bool _installed = false;
	QVersionNumber _version {1, 0, 0};

	bool runChoco(QStringList arguments, bool source, bool verify = true);
};

#endif // TESTINSTALLER_H
