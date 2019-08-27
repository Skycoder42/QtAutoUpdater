TEMPLATE = app

QT += testlib autoupdatercore autoupdatercore-private
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_updateinstaller

HEADERS +=

SOURCES += \
	tst_updateinstaller.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../../testrun.pri)
