TEMPLATE = subdirs

CONFIG += no_docs_target

SUBDIRS += \
#	auto \
#	installer

auto.depends += installer

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
