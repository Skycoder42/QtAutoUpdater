#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//Translate app
	QTranslator translator;
	translator.load(QLocale(),
					QStringLiteral("qt"),
					QStringLiteral("_"),
					QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&translator);

	//Translate updater
	QTranslator translatorUpdater;
	translatorUpdater.load(QLocale(),
						   QStringLiteral("qtautoupdatergui"),
						   QStringLiteral("_"),
						   QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&translatorUpdater);

	//Launch GUI
	MainWindow w;
	w.show();

	return a.exec();
}
