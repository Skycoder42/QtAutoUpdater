TARGET = QtAutoUpdaterGui

QT = core gui widgets autoupdatercore autoupdatercore-private

win32 {
	QT += winextras
	LIBS += -lAdvapi32 -lOle32 -lShell32
} else:mac {
	LIBS += -framework Security
} else:unix {
	LIBS += -lutil
}

include(../3rdparty/vendor/vendor.pri)

HEADERS += \
	qautoupdatergui_global.h \
	updatebutton_p.h \
	updatebutton.h \
	updatecontroller_p.h \
	updatecontroller.h \
	adminauthorization_p.h \
	progressdialog_p.h \
	updateinfodialog_p.h

SOURCES += \
	progressdialog.cpp \
	updatebutton.cpp \
	updatecontroller.cpp \
	updateinfodialog.cpp

win32: SOURCES += adminauthorization_win.cpp
else:mac: SOURCES += adminauthorization_mac.cpp
else:unix: SOURCES += adminauthorization_x11.cpp

FORMS += \
	progressdialog.ui \
	updatebutton.ui \
	updateinfodialog.ui

RESOURCES += \
	autoupdatergui_resource.qrc

include(./translations/translations.pri)

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterGui"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
