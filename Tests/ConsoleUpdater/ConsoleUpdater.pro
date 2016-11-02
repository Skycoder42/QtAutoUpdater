QT += core
QT -= gui

CONFIG += c++11

TARGET = ConsoleUpdater
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/release/ -lQtAutoUpdaterCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/debug/ -lQtAutoUpdaterCore
else:mac: LIBS += -F$$OUT_PWD/../../AutoUpdaterCore/ -framework QtAutoUpdaterCore
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdaterCore

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater
