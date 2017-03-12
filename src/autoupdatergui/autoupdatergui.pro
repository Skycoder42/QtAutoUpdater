TARGET = QtAutoUpdaterGui

QT = core autoupdatercore

HEADERS += \
	qautoupdatergui_global.h

SOURCES +=

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterGui"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
