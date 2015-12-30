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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/release/ -lAutoUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/debug/ -lAutoUpdaterd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/libAutoUpdater.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/libAutoUpdaterd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/AutoUpdater.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/AutoUpdaterd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater.a

HEADERS += \
    testmanager.h
