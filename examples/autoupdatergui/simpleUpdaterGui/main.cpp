#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	
    //Translate app
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    a.installTranslator(&translator);

    //Translate updater
    QTranslator translatorUpdater;

    translatorUpdater.load(QString("translations/qtautoupdatergui_") + locale + QString(".qm"));

    a.installTranslator(&translatorUpdater);

    //Launch GUI
    MainWindow w;

    w.show();

    return a.exec();
}
