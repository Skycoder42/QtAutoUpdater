TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore \
	autoupdaterwidgets \
	imports \
	plugins \
	translations

android: SUBDIRS += jar

plugins.depends += autoupdatercore
autoupdaterwidgets.depends += autoupdatercore
imports.depends += autoupdatercore

lupdate.target = lupdate
lupdate.CONFIG = recursive
lupdate.recurse_target = lupdate
lupdate.recurse += translations
QMAKE_EXTRA_TARGETS += lupdate

QMAKE_EXTRA_TARGETS += run-tests
