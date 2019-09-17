#ifndef QPLAYSTOREUPDATEINSTALLER_H
#define QPLAYSTOREUPDATEINSTALLER_H

#include <QtAutoUpdaterCore/UpdateInstaller>

#include <QtAndroidExtras/QAndroidJniObject>

class QPlayStoreUpdaterBackend;
class QPlayStoreUpdateInstaller : public QtAutoUpdater::UpdateInstaller
{
	Q_OBJECT

public:
	enum InstallStatus : int {
		Unknown = 0x00000000,
		Pending = 0x00000001,
		Downloading = 0x00000002,
		Installing = 0x00000003,
		Installed = 0x00000004,
		Failed = 0x00000005,
		Canceled = 0x00000006,
		RequiresUiIntent = 0x0000000a,
		Downloaded = 0x0000000b
	};
	Q_ENUM(InstallStatus)

	enum InstallErrorCode : int {
		NoError = 0x00000000,
		NoErrorPartiallyAllowed = 0x00000001,
		ErrorUnknown = static_cast<int>(0xfffffffe),
		ErrorApiNotAvailable = static_cast<int>(0xfffffffd),
		ErrorInvalidRequest = static_cast<int>(0xfffffffc),
		ErrorInstallUnavailable = static_cast<int>(0xfffffffb),
		ErrorInstallNotAllowed = static_cast<int>(0xfffffffa),
		ErrorDownloadNotPresent = static_cast<int>(0xfffffff9),
		ErrorInstallInProgress = static_cast<int>(0xfffffff8),
		ErrorInternalError = static_cast<int>(0xffffff9c)
	};
	Q_ENUM(InstallErrorCode)

	explicit QPlayStoreUpdateInstaller(const QAndroidJniObject &updateHelper,
									   QPlayStoreUpdaterBackend *parent = nullptr);

	Features features() const override;

public Q_SLOTS:
	void cancelInstall() override;
	void eulaHandled(const QVariant &id, bool accepted) override;
	void restartApplication() override;

protected:
	void startInstallImpl() override;

private:
	friend class QPlayStoreUpdaterBackend;

	QPlayStoreUpdaterBackend *_backend;
	QAndroidJniObject _updateHelper;
	bool _canceled = false;
	bool _done = false;

	void onStateUpdate(const QAndroidJniObject &state);
	static QString errorMessage(InstallErrorCode code);
};

#endif // QPLAYSTOREUPDATEINSTALLER_H
