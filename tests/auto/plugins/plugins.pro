TEMPLATE = subdirs

SUBDIRS += \
	testlib \
	QtIfwTest

for(var, SUBDIRS):!equals(var, testlib): $${var}.depends += testlib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
