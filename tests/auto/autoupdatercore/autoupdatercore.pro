TEMPLATE = subdirs

SUBDIRS += QtIfwPlgTest \
	test-plugin

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
