TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += autoupdatercore \
	autoupdatergui

autoupdatergui.depends += autoupdatercore

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
