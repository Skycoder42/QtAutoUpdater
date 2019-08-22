TEMPLATE = subdirs

SUBDIRS += \
	SchedulerTest \
	test-plugin \
	UpdaterTest \
#	QtIfwPlgTest

UpdaterTest.depends += test-plugin

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
