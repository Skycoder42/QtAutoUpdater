TEMPLATE = app

QT = core gui testlib autoupdatercore network httpserver

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_webquery

HEADERS += \
	updaterserver.h

SOURCES += \
	tst_webquery.cpp \
	updaterserver.cpp

DISTFILES += \
	installer.py

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../testlib/testlib.pri)

include($$PWD/../../testrun.pri)
