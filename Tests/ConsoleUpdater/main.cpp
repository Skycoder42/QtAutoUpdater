#include <QCoreApplication>
#include <QDebug>
#include <autoupdater.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	AutoUpdater *updater = new AutoUpdater(/*&a*/NULL);
	updater->setMaintenanceToolPath("C:/Program Files/IcoDroid/maintenancetool.exe");
	updater->runUpdaterOnExit();

	QObject::connect(updater, &AutoUpdater::checkUpdatesDone, [updater](bool a, bool b){
		qDebug() << "Has updates:" << a << "\nHas errors:" << b;
		qDebug() << updater->updateInfo();
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
