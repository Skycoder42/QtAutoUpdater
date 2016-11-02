#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T13:57:40
#
#-------------------------------------------------

TEMPLATE = lib
QT       += widgets
CONFIG += C++11

TARGET = QtAutoUpdaterGui
VERSION = 2.0.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtAutoUpdater GUI Library"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
	QT += winextras
	LIBS += -lAdvapi32 -lOle32 -lShell32
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.Skycoder42."
	QMAKE_FRAMEWORK_BUNDLE_NAME = "QtAutoUpdaterGui"

	CONFIG += lib_bundle
	LIBS += -framework Security
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''
} else:unix {
	LIBS += -lutil
	QMAKE_LFLAGS += '-Wl,-rpath,\'.\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../lib\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/lib\''
}

DEFINES += QTAUTOUPDATER_LIBRARY

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
