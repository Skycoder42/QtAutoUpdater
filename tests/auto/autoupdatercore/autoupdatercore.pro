TEMPLATE = subdirs

SUBDIRS += UpdaterTest

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
