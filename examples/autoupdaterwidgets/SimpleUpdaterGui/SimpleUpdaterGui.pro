TEMPLATE = app

QT += core gui widgets autoupdaterwidgets

TARGET = SimpleUpdaterGui

DEFINES += "CONFIG_PATH=\\\"$$PWD\\\""

HEADERS += \
	mainwindow.h

SOURCES += \
	main.cpp \
	mainwindow.cpp

FORMS += \
	mainwindow.ui

DISTFILES += \
	example.conf

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdaterwidgets/$$TARGET
!install_ok: INSTALLS += target
