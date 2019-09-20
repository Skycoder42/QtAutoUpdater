TEMPLATE = subdirs

SUBDIRS += \
	autoupdatercore

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
