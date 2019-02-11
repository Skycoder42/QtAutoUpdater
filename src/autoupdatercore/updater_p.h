#ifndef QTAUTOUPDATER_UPDATER_P_H
#define QTAUTOUPDATER_UPDATER_P_H

#include "qtautoupdatercore_global.h"
#include "updater.h"
#include "simplescheduler_p.h"

#include <QtCore/QProcess>
#include <QtCore/QLoggingCategory>

#include <qexceptionbase.h>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERCORE_EXPORT UpdaterPrivate : public QObject
{
	Q_DISABLE_COPY(UpdaterPrivate)
public:
	class Q_AUTOUPDATERCORE_EXPORT NoUpdatesXmlException : public QExceptionBase {
	public:
		const char *what() const noexcept override;

		void raise() const override;
		Base *clone() const override;
	};

	class Q_AUTOUPDATERCORE_EXPORT InvalidXmlException : public QExceptionBase {
	public:
		const char *what() const noexcept override;

		void raise() const override;
		Base *clone() const override;
	};

	Updater *q;

	QString toolPath;
	QList<Updater::UpdateInfo> updateInfos;
	bool normalExit = true;
	int lastErrorCode = EXIT_SUCCESS;
	QByteArray lastErrorLog;

	bool running = false;
	QProcess *mainProcess = nullptr;

	SimpleScheduler *scheduler;

	bool runOnExit = false;
	QStringList runArguments;
	QScopedPointer<AdminAuthoriser> adminAuth;

	UpdaterPrivate(Updater *q_ptr);
	~UpdaterPrivate() override;

	static const QString toSystemExe(QString basePath);

	bool startUpdateCheck();
	void stopUpdateCheck(int delay, bool async);
	QList<Updater::UpdateInfo> parseResult(const QByteArray &output);

public Q_SLOTS:
	void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
	void updaterError(QProcess::ProcessError error);

	void appAboutToExit();
};

}

Q_AUTOUPDATERCORE_EXPORT Q_DECLARE_LOGGING_CATEGORY(logQtAutoUpdater)

#endif // QTAUTOUPDATER_UPDATER_P_H
