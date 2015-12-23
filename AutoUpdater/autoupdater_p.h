#ifndef AUTOUPDATER_P_H
#define AUTOUPDATER_P_H

#include "autoupdater.h"
#include <QTimer>
#include <QProcess>
#include <exception>

template<typename... Args> struct SELECT {
	template<typename C, typename R>
	static Q_DECL_CONSTEXPR auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
		return pmf;
	}
};

class AutoUpdaterPrivate : public QObject
{
private:
	class UpdateParseException : public std::exception {};
	class NoUpdatesXmlException : public UpdateParseException {
	public:
		const char *what() const Q_DECL_OVERRIDE {
			return "The <updates> node could not be found";
		}
	};
	class InvalidXmlException : public UpdateParseException {
	public:
		const char *what() const Q_DECL_OVERRIDE {
			return "The found XML-part is not of a valid updates-XML-format";
		}
	};

	AutoUpdater *q_ptr;
	Q_DECLARE_PUBLIC(AutoUpdater)

	QString toolPath;
	QList<AutoUpdater::UpdateInfo> updateInfos;
	bool normalExit;
	int lastErrorCode;
	QByteArray lastErrorLog;

	bool running;
	QProcess *mainProcess;

	QHash<int, bool> activeTimers;

	bool runOnExit;
	QStringList runArguments;
	bool runAdmin;

	AutoUpdaterPrivate(AutoUpdater *q_ptr);
	~AutoUpdaterPrivate();

	static const QString toSystemExe(const QString basePath);

	bool startUpdateCheck();
	void stopUpdateCheck(int delay);
	void updaterReady(int exitCode);
	void updaterError(QProcess::ProcessError error);
	QList<AutoUpdater::UpdateInfo> parseResult(const QByteArray &output);

	void appAboutToExit();

	// QObject interface
protected:
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};

#endif // AUTOUPDATER_P_H
