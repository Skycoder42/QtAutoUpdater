#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:57:40
#
#-------------------------------------------------

QT       += widgets
CONFIG += C++11

TARGET = $$qtLibraryTarget(AutoUpdaterWidgets)
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    updatecontroller.cpp \
    progressdialog.cpp \
    updateinfodialog.cpp \
	updatepanel.cpp

win32 {
	SOURCES += adminauthorization_win.cpp

	LIBS += -lAdvapi32 -lOle32
} else:mac: SOURCES += adminauthorization_mac.cpp
else:unix: SOURCES += adminauthorization_x11.cpp

HEADERS += \
    updatecontroller.h \
	updatecontroller_p.h \
    progressdialog.h \
    updateinfodialog.h \
    updatepanel.h \
    messagemaster.h \
    adminauthorization.h

INCLUDEPATH += $$PWD/../AutoUpdater
DEPENDPATH += $$PWD/../AutoUpdater

FORMS += \
    progressdialog.ui \
    updateinfodialog.ui \
    updatepanel.ui

RESOURCES += \
    autoupdaterwidgets_resource.qrc
