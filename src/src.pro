TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += autoupdatercore \
	autoupdatergui

autoupdatergui.depends += autoupdatercore
autoupdatercore.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
