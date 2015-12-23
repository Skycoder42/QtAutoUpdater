#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QObject>

namespace QtAutoUpdater
{
	class UpdateControllerPrivate;
	class UpdateController : public QObject
	{
		Q_OBJECT

		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath WRITE setMaintenanceToolPath)
		Q_PROPERTY(DisplayLevel displayLevel READ displayLevel WRITE setDisplayLevel)
		Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)

	public:
		enum DisplayLevel {
			AutomaticLevel = 0,
			ExitLevel = 1,
			InfoLevel = 2,
			ExtendedInfoLevel = 3,
			ProgressLevel = 4,
			AskLevel = 5
		};
		Q_ENUM(DisplayLevel)

		explicit UpdateController(QWidget *parentWindow = 0);
		~UpdateController();

		QString maintenanceToolPath() const;
		void setMaintenanceToolPath(QString maintenanceToolPath);
		DisplayLevel displayLevel() const;
		void setDisplayLevel(DisplayLevel displayLevel);
		bool isRunning() const;

	public slots:
		bool start();
		bool cancelUpdate(int maxDelay = 3000);

	signals:
		void runningChanged(bool running);

	private slots:
		void checkUpdatesDone(bool hasUpdates, bool hasError);

	private:
		UpdateControllerPrivate *d_ptr;
		Q_DECLARE_PRIVATE(UpdateController)
	};
}

#endif // UPDATECONTROLLER_H
