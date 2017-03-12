TARGET = QtAutoUpdaterCore

QT = core

HEADERS += \
	qautoupdatercore_global.h \
    adminauthoriser.h \
    updater_p.h \
    updater.h \
    simplescheduler_p.h

SOURCES += \
    simplescheduler.cpp \
    updater_p.cpp \
    updater.cpp

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterCore"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
