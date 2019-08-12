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

	auto updater = QtAutoUpdater::Updater::createQtIfwUpdater(a.arguments().at(1));
	if (!updater){
		qCritical() << "Plugin not available";
		return EXIT_FAILURE;
	}
	QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](QtAutoUpdater::Updater::Result result){
		qInfo() << "Has updates:" << result
				 << "\nError Message:" << updater->errorMessage()
				 << "\nUpdate List:" << updater->updateInfo();
		if(result == QtAutoUpdater::Updater::Result::NewUpdates)
			updater->runUpdater();
		qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
