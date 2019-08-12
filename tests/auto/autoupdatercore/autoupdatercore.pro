TEMPLATE = subdirs

SUBDIRS += QtIfwPlgTest

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
