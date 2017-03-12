TEMPLATE = subdirs

SUBDIRS = autoupdatercore autoupdatergui

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
