#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:59:22
#
#-------------------------------------------------
TEMPLATE = app

QT       += core gui widgets autoupdatergui
CONFIG += C++11

TARGET = WidgetsUpdater

SOURCES += main.cpp\
		mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
	main_res.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdatergui/WidgetsUpdater
INSTALLS += target

#not found by linker?
unix:!mac {
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

#add lib dir to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''
