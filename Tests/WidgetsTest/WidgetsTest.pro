#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:59:22
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WidgetsTest
TEMPLATE = app

include(../../AutoUpdaterGui/DialogMaster/dialogmaster.pri)

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/release/ -lQtAutoUpdaterController
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/debug/ -lQtAutoUpdaterControllerd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterController
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterController_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterController

INCLUDEPATH += $$PWD/../../AutoUpdaterGui
DEPENDPATH += $$PWD/../../AutoUpdaterGui

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/release/libQtAutoUpdaterController.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/debug/libQtAutoUpdaterControllerd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/release/QtAutoUpdaterController.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/debug/QtAutoUpdaterControllerd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/libQtAutoUpdaterController.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/libQtAutoUpdaterController_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterGui/libQtAutoUpdaterController.a

win32: QT += winextras
win32: LIBS += -lAdvapi32 -lOle32
else:mac: LIBS += -framework Security
else:unix: LIBS += -lutil

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/release/ -lQtAutoUpdater
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/debug/ -lQtAutoUpdaterd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterCore/ -lQtAutoUpdater

INCLUDEPATH += $$PWD/../../AutoUpdaterCore
DEPENDPATH += $$PWD/../../AutoUpdaterCore

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/libQtAutoUpdater.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/libQtAutoUpdaterd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/release/QtAutoUpdater.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/debug/QtAutoUpdaterd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../AutoUpdaterCore/libQtAutoUpdater.a

RESOURCES += \
    main_res.qrc
