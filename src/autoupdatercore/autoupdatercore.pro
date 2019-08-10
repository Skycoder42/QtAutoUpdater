TARGET = QtAutoUpdaterCore

QT = core core-private

HEADERS += \
	adminauthoriser.h \
	updater_p.h \
	updater.h \
	simplescheduler_p.h \
	qtautoupdatercore_global.h \
	updaterbackend.h \
	updaterbackend_p.h \
	updaterplugin.h

SOURCES += \
	simplescheduler.cpp \
	updater_p.cpp \
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

QDEP_DEPENDS += Skycoder42/QExceptionBase@1.0.0
QDEP_EXPORTS += Skycoder42/QExceptionBase@1.0.0
CONFIG += qdep_no_link

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")
