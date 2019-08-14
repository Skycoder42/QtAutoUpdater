TARGET = QtAutoUpdaterCore

QT = core core-private

HEADERS += \
	adminauthoriser.h \
	updateinfo.h \
	updateinfo_p.h \
	updater.h \
	updater_p.h \
	simplescheduler_p.h \
	qtautoupdatercore_global.h \
	updaterbackend.h \
	updaterbackend_p.h \
	updaterplugin.h

SOURCES += \
	simplescheduler.cpp \
	updateinfo.cpp \
	updater.cpp \
	adminauthoriser.cpp \
	updaterbackend.cpp \
	updaterplugin.cpp

load(qt_module)

QMAKE_EXTRA_TARGETS += lrelease

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterCore"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
