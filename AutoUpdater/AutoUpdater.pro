#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T17:36:25
#
#-------------------------------------------------

QT -= gui
CONFIG += C++11

TARGET = $$qtLibraryTarget(AutoUpdater)
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    updater.cpp \
    updater_p.cpp \
    updatescheduler.cpp

HEADERS += \
    updater.h \
    updater_p.h \
	adminauthoriser.h \
    updatescheduler.h \
    updatescheduler_p.h \
    updatetask.h

