TARGET = qhomebrew

QT = autoupdatercore autoupdatercore-private

HEADERS += \
	qhomebrewupdateinstaller.h \
	qhomebrewupdaterbackend.h \
	qhomebrewupdaterplugin.h

SOURCES += \
	qhomebrewupdateinstaller.cpp \
	qhomebrewupdaterbackend.cpp \
	qhomebrewupdaterplugin.cpp

DISTFILES += qhomebrew.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QHomebrewUpdaterPlugin
load(qt_plugin)
