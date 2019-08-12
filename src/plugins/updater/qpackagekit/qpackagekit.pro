TARGET = qpackagekit

QT = autoupdatercore autoupdatercore-private
CONFIG += link_pkgconfig
PKGCONFIG += packagekitqt5

HEADERS += \
	qpackagekitupdaterbackend.h \
	qpackagekitupdaterplugin.h

SOURCES += \
	qpackagekitupdaterbackend.cpp \
	qpackagekitupdaterplugin.cpp

DISTFILES += qpackagekit.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QPackageKitUpdaterPlugin
load(qt_plugin)
