TARGET = QtAutoUpdaterCore

QT = core core-private

HEADERS += \
	adminauthoriser.h \
	updateinfo.h \
	updateinfo_p.h \
	updateinstaller.h \
	updateinstaller_p.h \
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
	updateinstaller.cpp \
	updater.cpp \
	updaterbackend.cpp \
	updaterplugin.cpp

win32:!winrt: SOURCES += adminauthoriser_win.cpp
else:mac:!ios: SOURCES += adminauthoriser_mac.cpp
else:unix:qtConfig(process): SOURCES += adminauthoriser_x11.cpp
else: SOURCES += adminauthoriser_dummy.cpp

load(qt_module)

win32:!winrt: LIBS += -lAdvapi32 -lOle32 -lShell32
else:mac:!ios: LIBS += -framework Security

QMAKE_EXTRA_TARGETS += lrelease

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
