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

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    main_res.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/release/ -lQtAutoUpdaterGui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/debug/ -lQtAutoUpdaterGuid
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterGui
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterGui_debug
else:unix: LIBS += -L$$OUT_PWD/../../AutoUpdaterGui/ -lQtAutoUpdaterGui

INCLUDEPATH += $$PWD/../../AutoUpdater
DEPENDPATH += $$PWD/../../AutoUpdater
INCLUDEPATH += $$PWD/../../AutoUpdaterGui
DEPENDPATH += $$PWD/../../AutoUpdaterGui
