#ifndef QTESTUPDATERBACKEND_H
#define QTESTUPDATERBACKEND_H

#include <QtCore/QTimer>
#include <QtCore/QTimer>

#include <QtAutoUpdaterCore/UpdaterBackend>

class QTestUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	explicit QTestUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	SecondaryInfo secondaryInfo() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

protected:
	bool initialize() override;

private Q_SLOTS:
	void timerTriggered();

private:
	QTimer *_timer;
	bool _updating = false;
	quint8 _tCounter = 0;
};

Q_DECLARE_LOGGING_CATEGORY(logTestPlugin)

#endif // QTESTUPDATERBACKEND_H
