TEMPLATE = app

QT += testlib autoupdatercore autoupdatercore-private
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_scheduler

HEADERS +=

SOURCES += \
	tst_scheduler.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include($$PWD/../../testrun.pri)
