TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
 \#	autoupdatergui
	plugins

plugins.depends += autoupdatercore
autoupdatergui.depends += autoupdatercore

QMAKE_EXTRA_TARGETS += run-tests
