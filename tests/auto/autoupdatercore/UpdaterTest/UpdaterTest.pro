TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += C++11 console
CONFIG -= app_bundle

TARGET = tst_updatertest

SOURCES += tst_updatertest.cpp \
	installercontroller.cpp

HEADERS += \
	installercontroller.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += BINDIR=\\\"$$[QT_INSTALL_BINS]/../../../Tools/QtInstallerFramework/3.0/bin/\\\"
DEFINES += QTDIR=\\\"$$[QT_INSTALL_BINS]/../../../\\\"

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''
