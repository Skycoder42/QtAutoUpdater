TEMPLATE = app

QT += core gui widgets autoupdatergui

CONFIG += C++11

TARGET = SimpleUpdaterGui

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

target.path = $$[QT_INSTALL_EXAMPLES]/autoupdatergui/$$TARGET
INSTALLS += target
