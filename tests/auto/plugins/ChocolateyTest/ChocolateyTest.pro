TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_chocolatey

HEADERS += \
	testinstaller.h

SOURCES += \
	testinstaller.cpp \
	tst_chocolatey.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../testlib/testlib.pri)

include($$PWD/../../testrun.pri)
