TEMPLATE = subdirs

SUBDIRS += testlib 

qtHaveModule(httpserver): SUBDIRS += WebQueryTest
qtConfig(process): SUBDIRS += QtIfwTest
win32:qtConfig(process): SUBDIRS += ChocolateyTest
mac:!ios:qtConfig(process): SUBDIRS += HomebrewTest

for(var, SUBDIRS):!equals(var, testlib): $${var}.depends += testlib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
