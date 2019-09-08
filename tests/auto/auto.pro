TEMPLATE = subdirs

SUBDIRS += \
	cmake \
	autoupdatercore \
	autoupdaterwidgets \
	plugins

plugins.depends += autoupdatercore

cmake.CONFIG += no_run-tests_target
prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
