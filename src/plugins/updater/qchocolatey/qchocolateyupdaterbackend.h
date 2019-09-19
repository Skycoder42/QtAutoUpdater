#ifndef QCHOCOLATEYUPDATERBACKEND_H
#define QCHOCOLATEYUPDATERBACKEND_H

#include <QtCore/QLoggingCategory>

#include <QtAutoUpdaterCore/ProcessBackend>

class QChocolateyUpdaterBackend : public QtAutoUpdater::ProcessBackend
{
	Q_OBJECT

public:
	explicit QChocolateyUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	SecondaryInfo secondaryInfo() const override;
	void checkForUpdates() override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;
	void onToolDone(int id, int exitCode, QIODevice *processDevice) override;
	std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

private:
	static const QString KeyPackages;
	static const QString KeyPath;
	static const QString KeyExtraCheckArgs;
	static const QString KeyGuiExePath;
	static const QString KeyExtraGuiArgs;
	static const QString KeyRunAsAdmin;

	static const QString DefaultGuiExePath;
	static constexpr bool DefaultRunAsAdmin {true};

	QProcess *_chocoProc = nullptr;
	QStringList _packages;

	QString chocoPath() const;
	QString guiPath() const;
};

Q_DECLARE_LOGGING_CATEGORY(logChocoBackend)

#endif // QCHOCOLATEYUPDATERBACKEND_H
