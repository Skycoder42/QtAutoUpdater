#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T17:56:29
#
#-------------------------------------------------

QT += testlib
CONFIG += C++11

QT -= gui

TARGET = tst_updatertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_updatertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/release/ -lQtAutoUpdaterCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/debug/ -lQtAutoUpdaterCore
else:mac: LIBS += -F$$OUT_PWD/../../AutoUpdaterCore/ -framework QtAutoUpdaterCore
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdaterCore

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater
