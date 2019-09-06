TARGET = QtAutoUpdaterWidgets

QT = core gui widgets autoupdatercore autoupdatercore-private widgets-private gui-private core-private
# from taskbar control, must be done here because load(qt_module) preceds load(qdep)
win32: QT += winextras
else:mac: QT += macextras
else:linux:!android:!emscripten: QT += dbus

HEADERS += \
	installwizard_p.h \
	qtautoupdaterwidgets_global.h \
	updatebutton_p.h \
	updatebutton.h \
	updatecontroller_p.h \
	updatecontroller.h \
	progressdialog_p.h \
	updateinfodialog_p.h

SOURCES += \
	installwizard.cpp \
	progressdialog.cpp \
	updatebutton.cpp \
	updatecontroller.cpp \
	updateinfodialog.cpp

FORMS += \
	componentspage.ui \
	errorpage.ui \
	installpage.ui \
	progressdialog.ui \
	successpage.ui \
	updatebutton.ui \
	updateinfodialog.ui

RESOURCES += \
	autoupdaterwidgets.qrc

TRANSLATIONS += \
	translations/qtautoupdaterwidgets_de.ts \
	translations/qtautoupdaterwidgets_es.ts \
	translations/qtautoupdaterwidgets_fr.ts \
	translations/qtautoupdaterwidgets_template.ts

DISTFILES += $$TRANSLATIONS

load(qt_module)

CONFIG += lrelease
QM_FILES_INSTALL_PATH = $$[QT_INSTALL_TRANSLATIONS]

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

QDEP_DEPENDS += \
	Skycoder42/DialogMaster@1.4.0 \
	Skycoder42/QTaskbarControl@1.3.0

!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

#replace template qm by ts
QM_FILES -= $$__qdep_lrelease_real_dir/qtautoupdaterwidgets_template.qm
QM_FILES += translations/qtautoupdaterwidgets_template.ts
