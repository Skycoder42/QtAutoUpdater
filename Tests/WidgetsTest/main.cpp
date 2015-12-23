#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QApplication::setWindowIcon(QIcon(":/icons/main.ico"));
	MainWindow w;
	w.show();

	return a.exec();
}
