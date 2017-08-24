load(qt_parts)

SUBDIRS += doc

docTarget.target = doxygen
docTarget.CONFIG += recursive
docTarget.recurse_target = doxygen
QMAKE_EXTRA_TARGETS += docTarget

DISTFILES += .qmake.conf
