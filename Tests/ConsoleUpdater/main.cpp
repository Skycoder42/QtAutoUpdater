#include <QCoreApplication>
#include <QDebug>
#include <updater.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

#if defined(Q_OS_WIN32)
	QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater("C:/Program Files/IcoDroid/maintenancetool", NULL);
#elif defined(Q_OS_OSX)
	QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater("/Applications/IcoDroid.app/maintenancetool", NULL);
#elif defined(Q_OS_UNIX)
	QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater("/home/sky/IcoDroid/maintenancetool", NULL);
#endif
	updater->runUpdaterOnExit();

	QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](bool a, bool b){
		qDebug() << "Has updates:" << a << "\nHas errors:" << b;
		qDebug() << updater->updateInfo();
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
