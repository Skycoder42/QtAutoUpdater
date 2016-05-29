#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T17:36:25
#
#-------------------------------------------------

QT -= gui
CONFIG += C++11

TARGET = $$qtLibraryTarget(QtAutoUpdater)
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    updater.cpp \
    updater_p.cpp \
    simplescheduler.cpp

HEADERS += \
    updater.h \
    updater_p.h \
	adminauthoriser.h \
    simplescheduler.h

DISTFILES += \
    adminauthoriser.dox \
    updater.dox

