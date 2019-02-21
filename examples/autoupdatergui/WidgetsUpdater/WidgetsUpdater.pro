TEMPLATE = app

QT += core gui widgets autoupdatergui

TARGET = WidgetsUpdater

SOURCES += main.cpp \
	mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

RESOURCES += \
	main_res.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdatergui/$$TARGET
INSTALLS += target
