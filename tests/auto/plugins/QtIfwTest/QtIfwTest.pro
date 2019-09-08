TEMPLATE = app

QT += testlib autoupdatercore
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TARGET = tst_qtifw

HEADERS += \
	installercontroller.h

SOURCES += \
	installercontroller.cpp \
	tst_qtifw.cpp

DISTFILES += \
	installer/packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml \
	installer/config/config.xml \
	installer/config/controller.qs

isEmpty(QT_IFW_VERSION): QT_IFW_VERSION = 3.1

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += BINDIR=\\\"$$[QT_INSTALL_BINS]/../../../Tools/QtInstallerFramework/$$QT_IFW_VERSION/bin/\\\"
DEFINES += QTDIR=\\\"$$[QT_INSTALL_BINS]/../../../\\\"

message($$DEFINES)

include($$PWD/../testlib/testlib.pri)

include($$PWD/../../testrun.pri)
