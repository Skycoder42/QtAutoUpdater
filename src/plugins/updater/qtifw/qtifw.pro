TARGET = qtifw

QT = autoupdatercore autoupdatercore-private

HEADERS += \
	qtifwupdaterbackend.h \
	qtifwupdaterplugin.h

SOURCES += \
	qtifwupdaterbackend.cpp \
	qtifwupdaterplugin.cpp

DISTFILES += qtifw.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QtIfwUpdaterPlugin
load(qt_plugin)
