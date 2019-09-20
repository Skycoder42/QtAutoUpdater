TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
	autoupdaterquick

autoupdatercore.CONFIG += no_lupdate_target

prepareRecursiveTarget(lupdate)
QMAKE_EXTRA_TARGETS += lupdate
