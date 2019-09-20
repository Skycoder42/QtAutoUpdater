TEMPLATE = subdirs

SUBDIRS += \
	cmake \
	autoupdatercore \
	autoupdaterwidgets \
	qml

!no_plugin_tests: SUBDIRS += plugins

plugins.depends += autoupdatercore
qml.depends += autoupdatercore

cmake.CONFIG += no_run-tests_target
prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
