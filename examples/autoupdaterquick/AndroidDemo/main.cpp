#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtAutoUpdaterCore>
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	qputenv("QT_LOGGING_RULES", "qt.autoupdater.*.debug=true");

	QGuiApplication app(argc, argv);

	qInfo() << QtAutoUpdater::Updater::supportedUpdaterBackends();

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty(QStringLiteral("AppVersion"), QCoreApplication::applicationVersion());
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
