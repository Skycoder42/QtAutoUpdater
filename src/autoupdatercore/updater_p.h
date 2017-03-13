#ifndef UPDATER_P_H
#define UPDATER_P_H

#include "qautoupdatercore_global.h"
#include "updater.h"
#include "simplescheduler_p.h"

#include <QtCore/QProcess>
#include <QtCore/QLoggingCategory>
#include <QtCore/QException>

#include <exception>

namespace QtAutoUpdater
{

class Q_AUTOUPDATERCORE_EXPORT UpdaterPrivate : public QObject
{
public:
	class Q_AUTOUPDATERCORE_EXPORT NoUpdatesXmlException : public QException {
	public:
		const char *what() const noexcept override;

		void raise() const override;
		QException *clone() const override;
	};

	class Q_AUTOUPDATERCORE_EXPORT InvalidXmlException : public QException {
	public:
		const char *what() const noexcept override;

		void raise() const override;
		QException *clone() const override;
	};

	Updater *q;

	QString toolPath;
	QList<Updater::UpdateInfo> updateInfos;
	bool normalExit;
	int lastErrorCode;
	QByteArray lastErrorLog;

	bool running;
	QProcess *mainProcess;

	SimpleScheduler *scheduler;

	bool runOnExit;
	QStringList runArguments;
	QScopedPointer<AdminAuthoriser> adminAuth;

	UpdaterPrivate(Updater *q_ptr);
	~UpdaterPrivate();

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

#endif // UPDATER_P_H
