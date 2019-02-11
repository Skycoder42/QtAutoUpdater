TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_updatertest

SOURCES += tst_updatertest.cpp \
	installercontroller.cpp

HEADERS += \
	installercontroller.h

include($$PWD/../../testrun.pri)
