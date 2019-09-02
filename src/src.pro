TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
	autoupdaterwidgets \
	imports \
	plugins

plugins.depends += autoupdatercore
autoupdaterwidgets.depends += autoupdatercore
imports.depends += autoupdatercore

QMAKE_EXTRA_TARGETS += run-tests
