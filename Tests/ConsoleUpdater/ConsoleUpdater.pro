QT += core
QT -= gui

CONFIG += c++11

TARGET = ConsoleUpdater
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/release/ -lAutoUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdater/debug/ -lAutoUpdaterd
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdater/ -lAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/libAutoUpdater.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/libAutoUpdaterd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/release/AutoUpdater.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/debug/AutoUpdaterd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdater/libAutoUpdater.a
