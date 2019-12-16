#ifndef QWEBQUERYUPDATERBACKEND_H
#define QWEBQUERYUPDATERBACKEND_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QVariant>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaMethod>

#include <QtAutoUpdaterCore/UpdaterBackend>

#include <QtNetwork/QNetworkAccessManager>

class QWebQueryUpdaterBackend : public QtAutoUpdater::UpdaterBackend
{
	Q_OBJECT

public:
	struct Check {
		static const QString KeyUrl;
		static const QString KeyAutoQuery;
		static const QString KeySpdy;
		static const QString KeyHttp2;
		static const QString KeyHsts;
		static const QString KeySslConfiguration;
		struct Headers {
			static const QString KeySize;
			static const QString KeyKey;
			static const QString KeyValue;
		};
		static const QString KeyVerb;
		static const QString KeyBody;
		static const QString KeyParser;

		static constexpr bool DefaultAutoQuery {true};
		static const QByteArray DefaultVerb;
	};

	struct Install {
		static const QString KeyDownload;
		static const QString KeyTool;
		static const QString KeyParallel;
		static const QString KeyUrl;
		static const QString KeyPath;
		static const QString KeyArguments;
		static const QString KeyPwd;
		static const QString KeyRunAsAdmin;
		static const QString KeyAddDataArgs;
		static const QString KeyDownloadUrl;
		static const QString KeyUseInfoDownload;
		struct Headers {
			static const QString KeySize;
			static const QString KeyKey;
			static const QString KeyValue;
		};
		static const QString KeyExecDownload;

		static constexpr bool DefaultDownload {false};
		static constexpr bool DefaultParallel {false};
		static constexpr bool DefaultAddDataArgs {false};
		static constexpr bool DefaultUseInfoDownloads {false};
		static constexpr bool DefaultExecDownload {false};
	};

	explicit QWebQueryUpdaterBackend(QString &&key, QObject *parent = nullptr);

	Features features() const override;
	void checkForUpdates() override;
	void abort(bool force) override;
	bool triggerUpdates(const QList<QtAutoUpdater::UpdateInfo> &infos, bool track) override;
	QtAutoUpdater::UpdateInstaller *createInstaller() override;

#if QT_CONFIG(process)
	static std::optional<QString> testForProcess(IConfigReader *config);
	static bool runProcess(QObject *parent,
						   QMetaMethod doneSignal,
						   IConfigReader *config,
						   const QString &program,
						   const QList<QtAutoUpdater::UpdateInfo> &infos,
						   bool track,
						   const std::optional<QString> &replaceArg = std::nullopt);
#endif

Q_SIGNALS:
	void abortCheck(QPrivateSignal);

protected:
	bool initialize() override;

private Q_SLOTS:
	void handleReply();
	void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
	static const QString ParserAuto;
	static const QString ParserVersion;
	static const QString ParserJson;

	QNetworkAccessManager *_nam = nullptr;

	void addStandardQuery(QUrl &url) const;

	void parseResult(QNetworkReply *device);
	void parseJson(QNetworkReply *device);
	void parseVersion(QNetworkReply *device);

	QString requestUrl(QNetworkReply *reply) const;
};

Q_DECLARE_LOGGING_CATEGORY(logWebBackend)

#endif // QWEBQUERYUPDATERBACKEND_H
