#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QtAutoUpdaterCore/Updater>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	if(a.arguments().size() < 2) {
		qCritical() << "Usage: ConsoleUpdater <backend> [<args>] ...]";
		return EXIT_FAILURE;
	}

	const auto backend = a.arguments().at(1);
	QtAutoUpdater::Updater *updater = nullptr;
	if (backend == QStringLiteral("qtifw"))
		updater = QtAutoUpdater::Updater::createQtIfwUpdater(a.arguments().at(2));
	else /*if (backend == QStringLiteral("packagekit"))*/
		updater = QtAutoUpdater::Updater::createUpdater(backend);

	if (!updater) {
		qCritical() << "Plugin" << backend << "not available";
		return EXIT_FAILURE;
	}
	QObject::connect(updater, &QtAutoUpdater::Updater::progressChanged, [](double progress){
		qDebug() << static_cast<int>(progress * 100) << "%";
	});
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
