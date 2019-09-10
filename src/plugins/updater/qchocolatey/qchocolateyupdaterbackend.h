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
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	std::optional<UpdateProcessInfo> initializeImpl() override;
	void parseResult(int exitCode, QIODevice *processDevice) override;
	std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

private:
	QProcess *_chocoProc = nullptr;
	QStringList _packages;

	QString chocoPath(bool asGui) const;
};

Q_DECLARE_LOGGING_CATEGORY(logChocoBackend)

#endif // QCHOCOLATEYUPDATERBACKEND_H
