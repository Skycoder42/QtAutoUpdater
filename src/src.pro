TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
	autoupdaterwidgets \
	imports \
	plugins

android: SUBDIRS += jar

plugins.depends += autoupdatercore
autoupdaterwidgets.depends += autoupdatercore
imports.depends += autoupdatercore

plugins.CONFIG += no_lupdate_target
jar.CONFIG += no_lupdate_target

prepareRecursiveTarget(lupdate)
QMAKE_EXTRA_TARGETS += run-tests lupdate
