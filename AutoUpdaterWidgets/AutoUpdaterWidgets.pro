#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:57:40
#
#-------------------------------------------------

QT       += widgets
CONFIG += C++11

TARGET = $$qtLibraryTarget(AutoUpdaterWidgets)
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    updatecontroller.cpp

HEADERS += \
    updatecontroller.h \
	updatecontroller_p.h

INCLUDEPATH += $$PWD/../AutoUpdater
DEPENDPATH += $$PWD/../AutoUpdater
