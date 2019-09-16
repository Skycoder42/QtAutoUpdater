#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QtAutoUpdaterCore/Updater>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	if(a.arguments().size() < 2) {
		qCritical() << "Usage: ConsoleUpdater <backend> [<key>=<value>] ...]";
		return EXIT_FAILURE;
	}

	const auto backend = a.arguments().at(1);
	QVariantMap args;
	for (const auto &arg : a.arguments().mid(2)) {
		const auto eIdx = arg.indexOf(QLatin1Char('='));
		if (eIdx != -1)
			args.insert(arg.mid(0, eIdx), arg.mid(eIdx + 1));
	}
	auto updater = QtAutoUpdater::Updater::create(backend, args);

	if (!updater) {
		qCritical() << "Plugin" << backend << "not available";
		return EXIT_FAILURE;
	}
	QObject::connect(updater, &QtAutoUpdater::Updater::progressChanged, [](double progress, const QString &status){
		if (progress < 0.0) {
			qDebug().nospace().noquote() << status << " (...)";
		} else {
			qDebug().nospace().noquote() << status << " ("
										 << static_cast<int>(progress * 100) << "%)";
		}
	});
	QObject::connect(updater, &QtAutoUpdater::Updater::checkUpdatesDone, [updater](QtAutoUpdater::Updater::State state){
		qInfo() << "Update result:" << state;
		if(state == QtAutoUpdater::Updater::State::NewUpdates) {
			qInfo() << "Update list:" << updater->updateInfo();
			qInfo() << "Starting updater...";
			if (updater->runUpdater(QtAutoUpdater::Updater::InstallModeFlag::ForceOnExit))
				qApp->quit();
			else
				qApp->exit(EXIT_FAILURE);
		} else
			qApp->quit();
	});

	updater->checkForUpdates();
	return a.exec();
}
