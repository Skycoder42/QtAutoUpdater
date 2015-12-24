#ifndef UPDATECONTROLLER_H
#define UPDATECONTROLLER_H

#include <QObject>
#include <QAction>

namespace QtAutoUpdater
{
	class Updater;
	class UpdateControllerPrivate;
	class UpdateController : public QObject
	{
		Q_OBJECT

		Q_PROPERTY(QString maintenanceToolPath READ maintenanceToolPath CONSTANT)
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

		explicit UpdateController(QObject *parent = NULL);
		explicit UpdateController(QWidget *parentWindow);
		explicit UpdateController(const QString &maintenanceToolPath, QObject *parent = NULL);
		explicit UpdateController(const QString &maintenanceToolPath, QWidget *parentWindow);
		~UpdateController();

		QAction *getUpdateAction() const;
		QWidget *createUpdatePanel(QWidget *parentWidget);

		QString maintenanceToolPath() const;
		DisplayLevel currentDisplayLevel() const;
		bool isRunning() const;

		Updater *getUpdater() const;

	public slots:
		bool start(DisplayLevel displayLevel = ProgressLevel);
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
