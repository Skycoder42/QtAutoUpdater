#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <updater.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater(homePath + QStringLiteral("/QtAutoUpdaterTestInstaller/maintenancetool"),
																 nullptr);
	updater->runUpdaterOnExit();

	QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](bool a, bool b){
		qDebug() << "Has updates:" << a
				 << "\nHas errors:" << b
				 << "\nError string:" << updater->errorLog();
		qDebug() << updater->updateInfo();
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
