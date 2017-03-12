TARGET = QtAutoUpdater

QT = core


PUBLIC_HEADERS += \
	qautoupdater_global.h

PRIVATE_HEADERS +=

SOURCES +=

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdater"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
