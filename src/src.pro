TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
	autoupdatergui \
	imports \
	plugins

plugins.depends += autoupdatercore
autoupdatergui.depends += autoupdatercore
imports.depends += autoupdatercore autoupdatergui

QMAKE_EXTRA_TARGETS += run-tests
