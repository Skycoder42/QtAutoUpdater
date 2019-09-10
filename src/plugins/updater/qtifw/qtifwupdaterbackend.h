#ifndef QTIFWUPDATERBACKEND_H
#define QTIFWUPDATERBACKEND_H

#include <optional>

#include <QtCore/QFileInfo>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QLoggingCategory>

#include <QtAutoUpdaterCore/ProcessBackend>

class QtIfwUpdaterBackend : public QtAutoUpdater::ProcessBackend
{
	Q_OBJECT

public:
	explicit QtIfwUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	std::optional<UpdateProcessInfo> initializeImpl() override;
	void parseResult(int exitCode, QIODevice *processDevice) override;
	std::optional<InstallProcessInfo> installerInfo(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;

private:
	std::optional<QFileInfo> findMaintenanceTool();
	std::optional<QList<QtAutoUpdater::UpdateInfo>> parseUpdates(QIODevice *device);
	void checkReader(QXmlStreamReader &reader);
	Q_NORETURN void throwUnexpectedElement(QXmlStreamReader &reader);
};

Q_DECLARE_LOGGING_CATEGORY(logQtIfwBackend)

#endif // QTIFWUPDATERBACKEND_H
