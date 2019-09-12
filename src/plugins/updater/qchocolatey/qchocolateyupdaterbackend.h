#ifndef QCHOCOLATEYUPDATERBACKEND_H
#define QCHOCOLATEYUPDATERBACKEND_H

#include <QtCore/QLoggingCategory>

#include <QtAutoUpdaterCore/ProcessBackend>

class QChocolateyUpdaterBackend : public QtAutoUpdater::ProcessBackend
{
	Q_OBJECT

public:
	static const QString DefaultGuiPath;

	explicit QChocolateyUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;
	void onToolDone(int id, int exitCode, QIODevice *processDevice) override;
	std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

private:
	QProcess *_chocoProc = nullptr;
	QStringList _packages;

	QString chocoPath() const;
	QString guiPath() const;
};

Q_DECLARE_LOGGING_CATEGORY(logChocoBackend)

#endif // QCHOCOLATEYUPDATERBACKEND_H
