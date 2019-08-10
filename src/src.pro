TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
#	autoupdatergui

autoupdatergui.depends += autoupdatercore

QMAKE_EXTRA_TARGETS += run-tests
