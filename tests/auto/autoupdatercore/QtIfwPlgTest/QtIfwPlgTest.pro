TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_qtifwplg

SOURCES += \
	installercontroller.cpp \
	tst_qtifwplg.cpp

HEADERS += \
	installercontroller.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += BINDIR=\\\"$$[QT_INSTALL_BINS]/../../../Tools/QtInstallerFramework/3.1/bin/\\\"
DEFINES += QTDIR=\\\"$$[QT_INSTALL_BINS]/../../../\\\"

include($$PWD/../../testrun.pri)
