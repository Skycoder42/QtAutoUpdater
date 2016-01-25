#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setWindowIcon(QIcon(":/icons/main.ico"));
	MainWindow w;
	w.show();

	QTranslator tr;
	tr.load(QLocale(),
			"QtAutoUpdaterController",
			"_",
			QApplication::applicationDirPath());
	QApplication::installTranslator(&tr);

	return a.exec();
}
