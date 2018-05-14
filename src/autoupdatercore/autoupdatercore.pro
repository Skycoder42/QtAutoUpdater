TARGET = QtAutoUpdaterCore

QT = core

HEADERS += \
    adminauthoriser.h \
    updater_p.h \
    updater.h \
    simplescheduler_p.h \
    qtautoupdatercore_global.h

SOURCES += \
    simplescheduler.cpp \
    updater_p.cpp \
    updater.cpp

load(qt_module)

QMAKE_EXTRA_TARGETS += lrelease

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterCore"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
