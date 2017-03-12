TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += autoupdater

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
