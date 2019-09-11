#ifndef QHOMEBREWUPDATERBACKEND_H
#define QHOMEBREWUPDATERBACKEND_H

#include <QtCore/QLoggingCategory>

#include <QtAutoUpdaterCore/ProcessBackend>

class QHomebrewUpdaterBackend : public QtAutoUpdater::ProcessBackend
{
	Q_OBJECT

public:
	explicit QHomebrewUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	std::optional<UpdateProcessInfo> initializeImpl() override;
	void parseResult(int exitCode, QIODevice *processDevice) override;
	std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

private:
	QStringList _packages;

	QString cakebrewPath() const;
};

Q_DECLARE_LOGGING_CATEGORY(logBrewBackend)

#endif // QHOMEBREWUPDATERBACKEND_H
