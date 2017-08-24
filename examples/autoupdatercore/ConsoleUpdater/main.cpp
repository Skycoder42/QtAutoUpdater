#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QtAutoUpdaterCore/Updater>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	if(a.arguments().size() < 2) {
		qCritical() << "Usage: ConsoleUpdater <path_to_maintenancetool>";
		return EXIT_FAILURE;
	}

	QtAutoUpdater::Updater *updater = new QtAutoUpdater::Updater(a.arguments()[1],
																 nullptr);

	QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](bool hasUpdate, bool hasError){
		qDebug() << "Has updates:" << hasUpdate
				 << "\nHas errors:" << hasError
				 << "\nError string:" << updater->errorLog();
		qDebug() << updater->updateInfo();
		if(hasUpdate)
			updater->runUpdaterOnExit(QtAutoUpdater::Updater::PassiveUpdateArguments);
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
