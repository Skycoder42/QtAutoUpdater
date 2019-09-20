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

qtConfig(process) {
	HEADERS += \
		processbackend.h \
		processbackend_p.h
	SOURCES += \
		processbackend.cpp
}

win32:!winrt: SOURCES += adminauthoriser_win.cpp
else:mac:!ios: SOURCES += adminauthoriser_mac.cpp
else:unix:qtConfig(process): SOURCES += adminauthoriser_x11.cpp
else: SOURCES += adminauthoriser_dummy.cpp

android {
	ANDROID_BUNDLED_JAR_DEPENDENCIES = \
		jar/QtAutoUpdaterCorePlayStorePlugin.jar
	MODULE_DEFINES += "QT_AUTOUPDATER_PLAY_CORE_VERSION=\\\"$$PLAY_CORE_VERSION\\\""
}

TRANSLATIONS += \
	translations/qtautoupdatercore_de.ts \
	translations/qtautoupdatercore_es.ts \
	translations/qtautoupdatercore_fr.ts \
	translations/qtautoupdatercore_ar.ts \
	translations/qtautoupdatercore_template.ts

QDEP_LUPDATE_INPUTS += \
	$$PWD/../plugins \
	$$PWD/../jar \
	$$PWD/../imports/autoupdatercore \

load(qt_module)

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

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

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtautoupdatercore_template.qm
QM_FILES += translations/qtautoupdatercore_template.ts
