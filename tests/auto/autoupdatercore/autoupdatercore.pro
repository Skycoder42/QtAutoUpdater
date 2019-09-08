TEMPLATE = subdirs

SUBDIRS += \
	SchedulerTest \
	test-plugin \
	UpdateInstallerTest \
	UpdaterTest

UpdaterTest.depends += test-plugin
UpdateInstallerTest.depends += test-plugin

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
