#ifndef UPDATER_P_H
#define UPDATER_P_H

#include "updater.h"
#include <QProcess>
#include <QScopedPointer>
#include <exception>
#include "simplescheduler.h"

namespace QtAutoUpdater
{
	class UpdaterPrivate : public QObject
	{
	public:
		class UpdateParseException : public std::exception {};//TODO use QtException
		class NoUpdatesXmlException : public UpdateParseException {
		public:
			const char *what() const Q_DECL_NOEXCEPT Q_DECL_OVERRIDE {
				return "The <updates> node could not be found";
			}
		};
		class InvalidXmlException : public UpdateParseException {
		public:
			const char *what() const Q_DECL_NOEXCEPT Q_DECL_OVERRIDE {
				return "The found XML-part is not of a valid updates-XML-format";
			}
		};

		Updater *q_ptr;
		Q_DECLARE_PUBLIC(Updater)

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

	public slots:
		void updaterReady(int exitCode, QProcess::ExitStatus exitStatus);
		void updaterError(QProcess::ProcessError error);

		void appAboutToExit();
	};
}

#endif // UPDATER_P_H
