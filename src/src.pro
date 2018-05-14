TEMPLATE = subdirs

SUBDIRS += autoupdatercore \
	autoupdatergui

autoupdatergui.depends += autoupdatercore

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
