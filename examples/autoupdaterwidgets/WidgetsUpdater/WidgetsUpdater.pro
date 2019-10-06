TEMPLATE = app

QT += core gui widgets autoupdaterwidgets

TARGET = WidgetsUpdater

SOURCES += main.cpp \
	mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

RESOURCES += \
	main_res.qrc

DISTFILES += \
	example.conf

DEFINES += "EXAMPLE_PATH=\\\"$$PWD/example.conf\\\""

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdaterwidgets/$$TARGET
!install_ok: INSTALLS += target
