#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:57:40
#
#-------------------------------------------------

QT       += widgets
win32: QT += winextras
CONFIG += C++11

TARGET = $$qtLibraryTarget(QtAutoUpdaterController)
TEMPLATE = lib
CONFIG += staticlib

include(./DialogMaster/dialogmaster.pri)

INCLUDEPATH += $$PWD/../AutoUpdaterCore
DEPENDPATH += $$PWD/../AutoUpdaterCore

SOURCES += \
    updatecontroller.cpp \
    progressdialog.cpp \
    updateinfodialog.cpp \
	updatepanel.cpp

win32: SOURCES += adminauthorization_win.cpp
else:mac: SOURCES += adminauthorization_mac.cpp
else:unix: SOURCES += adminauthorization_x11.cpp

HEADERS += \
    updatecontroller.h \
	updatecontroller_p.h \
    progressdialog.h \
    updateinfodialog.h \
    updatepanel.h \
	adminauthorization.h \
    updatepanel_p.h

FORMS += \
    progressdialog.ui \
    updateinfodialog.ui \
    updatepanel.ui

RESOURCES += \
	autoupdatergui_resource.qrc

DISTFILES += \
    updatecontroller.dox

include(./translations/translations.pri)
