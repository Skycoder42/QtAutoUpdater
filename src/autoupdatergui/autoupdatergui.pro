TARGET = QtAutoUpdaterGui

QT = core gui widgets autoupdatercore autoupdatercore-private

win32 {
	QT += winextras
	LIBS += -lAdvapi32 -lOle32 -lShell32
} else:mac {
	LIBS += -framework Security
} else:unix {
	LIBS += -lutil
}

HEADERS += \
	updatebutton_p.h \
	updatebutton.h \
	updatecontroller_p.h \
	updatecontroller.h \
	adminauthorization_p.h \
	progressdialog_p.h \
	updateinfodialog_p.h \
	qtautoupdatergui_global.h

SOURCES += \
	progressdialog.cpp \
	updatebutton.cpp \
	updatecontroller.cpp \
	updateinfodialog.cpp

win32: SOURCES += adminauthorization_win.cpp
else:mac: SOURCES += adminauthorization_mac.cpp
else:unix: SOURCES += adminauthorization_x11.cpp

FORMS += \
	progressdialog.ui \
	updatebutton.ui \
	updateinfodialog.ui

RESOURCES += \
	autoupdatergui_resource.qrc

TRANSLATIONS += translations/qtautoupdatergui_de.ts \
	translations/qtautoupdatergui_es.ts \
	translations/qtautoupdatergui_template.ts

DISTFILES += $$TRANSLATIONS

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtAutoUpdaterGui"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

qpmx_ts_target.path = $$[QT_INSTALL_TRANSLATIONS]
qpmx_ts_target.depends += lrelease
INSTALLS += qpmx_ts_target

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)

#replace template qm by ts
qpmx_ts_target.files -= $$OUT_PWD/$$QPMX_WORKINGDIR/qtautoupdatergui_template.qm
qpmx_ts_target.files += translations/qtautoupdatergui_template.ts
