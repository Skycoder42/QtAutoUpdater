TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_homebrew

HEADERS += \
	testinstaller.h

SOURCES += \
	testinstaller.cpp \
	tst_homebrew.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../testlib/testlib.pri)

include($$PWD/../../testrun.pri)
