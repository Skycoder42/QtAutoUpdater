#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setApplicationName(QStringLiteral("WidgetsUpdater"));
	QApplication::setOrganizationName(QStringLiteral("Skycoder42"));
	QApplication::setOrganizationDomain(QStringLiteral("de.skycoder42"));
	QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/main.ico")));
	QApplication::setApplicationDisplayName(QStringLiteral("Widgets-Test"));
	MainWindow w;
	w.show();

	QTranslator tr;
	tr.load(QLocale(),
			QStringLiteral("qtautoupdatergui"),
			QStringLiteral("_"),
			QApplication::applicationDirPath());
	QApplication::installTranslator(&tr);

	return a.exec();
}
