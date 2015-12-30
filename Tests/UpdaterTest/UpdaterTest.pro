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


SOURCES += tst_updatertest.cpp \
    testmanager.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/release/ -lQtAutoUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/debug/ -lQtAutoUpdaterd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdaterCore
DEPENDPATH += $$PWD/../../AutoUpdaterCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/libQtAutoUpdater.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/libQtAutoUpdaterd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/QtAutoUpdater.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/QtAutoUpdaterd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater.a

HEADERS += \
    testmanager.h
