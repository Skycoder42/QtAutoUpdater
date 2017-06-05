TEMPLATE = subdirs

CONFIG += no_docs_target

SUBDIRS += auto \
	installer

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
