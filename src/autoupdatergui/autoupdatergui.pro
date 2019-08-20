TARGET = QtAutoUpdaterGui

QT = core gui widgets autoupdatercore autoupdatercore-private widgets-private gui-private core-private
# from taskbar control, must be done here because load(qt_module) preceds load(qdep)
win32: QT += winextras
else:mac: QT += macextras
else:linux:!android:!emscripten: QT += dbus

HEADERS += \
	updatebutton_p.h \
	updatebutton.h \
	updatecontroller_p.h \
	updatecontroller.h \
	progressdialog_p.h \
	updateinfodialog_p.h \
	qtautoupdatergui_global.h

SOURCES += \
	progressdialog.cpp \
	updatebutton.cpp \
	updatecontroller.cpp \
	updateinfodialog.cpp

FORMS += \
	progressdialog.ui \
	updatebutton.ui \
	updateinfodialog.ui

RESOURCES += \
	autoupdatergui_resource.qrc

TRANSLATIONS += \
	translations/qtautoupdatergui_de.ts \
	translations/qtautoupdatergui_es.ts \
	translations/qtautoupdatergui_fr.ts \
	translations/qtautoupdatergui_template.ts

DISTFILES += $$TRANSLATIONS

load(qt_module)

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterGui"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

QDEP_DEPENDS += \
	Skycoder42/DialogMaster@1.4.0 \
	Skycoder42/QTaskbarControl@1.2.2

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtautoupdatergui_template.qm
QM_FILES += translations/qtautoupdatergui_template.ts
