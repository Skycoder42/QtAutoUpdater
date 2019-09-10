TARGET = qchocolatey

QT = autoupdatercore autoupdatercore-private

HEADERS += \
	qchocolateyupdaterbackend.h \
	qchocolateyupdaterplugin.h

SOURCES += \
	qchocolateyupdaterbackend.cpp \
	qchocolateyupdaterplugin.cpp

DISTFILES += qchocolatey.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QChocolateyUpdaterPlugin
load(qt_plugin)
