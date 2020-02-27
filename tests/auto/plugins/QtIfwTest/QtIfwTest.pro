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

isEmpty(QT_IFW_VERSION): QT_IFW_VERSION = 3.2
isEmpty(QT_TOOL_PATH): QT_TOOL_PATH = $$[QT_INSTALL_BINS]/../../../Tools

DEFINES += SRCDIR=\\\"$$PWD\\\"
DEFINES += BINDIR=\\\"$$clean_path($$QT_TOOL_PATH/QtInstallerFramework/$$QT_IFW_VERSION/bin)\\\"
DEFINES += QTDIR=\\\"$$[QT_INSTALL_BINS]/../../..\\\"

include($$PWD/../testlib/testlib.pri)

include($$PWD/../../testrun.pri)
