TEMPLATE = subdirs

CONFIG += no_docs_target

SUBDIRS += auto \
	TestInstaller

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
