TARGET = qwebquery

QT = core gui autoupdatercore network autoupdatercore-private

HEADERS += \
	qwebqueryupdaterbackend.h \
	qwebqueryupdaterplugin.h

SOURCES += \
	qwebqueryupdaterbackend.cpp \
	qwebqueryupdaterplugin.cpp

qtConfig(process) {
	HEADERS += \
		qwebqueryupdateinstaller.h
	SOURCES += \
		qwebqueryupdateinstaller.cpp
}

DISTFILES += qwebquery.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QWebQueryUpdaterPlugin
load(qt_plugin)
