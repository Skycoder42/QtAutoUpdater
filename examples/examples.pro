TEMPLATE = subdirs

SUBDIRS = autoupdater

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget