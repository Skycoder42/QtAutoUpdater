TARGET = qwebquery

QT = core gui autoupdatercore network autoupdatercore-private

HEADERS += \
	qwebqueryupdateinstaller.h \
	qwebqueryupdaterbackend.h \
	qwebqueryupdaterplugin.h

SOURCES += \
	qwebqueryupdateinstaller.cpp \
	qwebqueryupdaterbackend.cpp \
	qwebqueryupdaterplugin.cpp

DISTFILES += qwebquery.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QWebQueryUpdaterPlugin
load(qt_plugin)
