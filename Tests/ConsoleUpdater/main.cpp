#include <QCoreApplication>
#include <QDebug>
#include <autoupdater.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	AutoUpdater *updater = new AutoUpdater(/*&a*/NULL);
#if defined(Q_OS_WIN32)
	updater->setMaintenanceToolPath("C:/Program Files/IcoDroid/maintenancetool");
#elif defined(Q_OS_OSX)
	updater->setMaintenanceToolPath("/Applications/IcoDroid.app/maintenancetool");
#elif defined(Q_OS_UNIX)
    updater->setMaintenanceToolPath("/home/sky/IcoDroid/maintenancetool");
#endif
	updater->runUpdaterOnExit();

	QObject::connect(updater, &AutoUpdater::checkUpdatesDone, [updater](bool a, bool b){
		qDebug() << "Has updates:" << a << "\nHas errors:" << b;
		qDebug() << updater->updateInfo();
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
