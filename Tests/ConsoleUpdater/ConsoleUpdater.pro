QT += core
QT -= gui

CONFIG += c++11

TARGET = ConsoleUpdater
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/release/ -lQtAutoUpdaterCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/debug/ -lQtAutoUpdaterCored
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdaterCore
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdaterCore_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/libQtAutoUpdaterCore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/libQtAutoUpdaterCored.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/QtAutoUpdaterCore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/QtAutoUpdaterCored.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdaterCore.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdaterCore_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdaterCore.a
