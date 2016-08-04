#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:57:40
#
#-------------------------------------------------

QT       += widgets
win32: QT += winextras
CONFIG += C++11

TARGET = $$qtLibraryTarget(QtAutoUpdaterGui)
TEMPLATE = lib

DEFINES += QTAUTOUPDATER_LIBRARY

win32 {
	QT += winextras
	LIBS += -lAdvapi32 -lOle32 -lShell32
} else:mac {
	LIBS += -framework Security
} else {
	LIBS += -lutil
}

include(../AutoUpdater/autoupdater.pri)
include(./QtUtils/DialogMaster/dialogmaster.pri)

HEADERS += \
	updatecontroller.h \
	updatecontroller_p.h \
	progressdialog.h \
	updateinfodialog.h \
	adminauthorization.h \
	updatebutton.h \
	updatebutton_p.h

SOURCES += \
    updatecontroller.cpp \
    progressdialog.cpp \
    updateinfodialog.cpp \
    updatebutton.cpp

win32: SOURCES += adminauthorization_win.cpp
else:mac: SOURCES += adminauthorization_mac.cpp
else:unix: SOURCES += adminauthorization_x11.cpp

FORMS += \
    progressdialog.ui \
    updateinfodialog.ui \
    updatebutton.ui

RESOURCES += \
	autoupdatergui_resource.qrc

DISTFILES += \
    updatecontroller.dox \
    updatebutton.dox

include(./translations/translations.pri)
