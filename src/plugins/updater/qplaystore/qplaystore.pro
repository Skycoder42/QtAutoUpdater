TARGET = qplaystore

QT = autoupdatercore androidextras autoupdatercore-private androidextras-private

HEADERS += \
	qplaystoreupdateinstaller.h \
	qplaystoreupdaterbackend.h \
	qplaystoreupdaterplugin.h

SOURCES += \
	qplaystoreupdateinstaller.cpp \
	qplaystoreupdaterbackend.cpp \
	qplaystoreupdaterplugin.cpp

DISTFILES += qplaystore.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QPlayStoreUpdaterPlugin
load(qt_plugin)
