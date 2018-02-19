load(qt_parts)

SUBDIRS += doc

doxygen.target = doxygen
doxygen.CONFIG = recursive
doxygen.recurse_target = doxygen
doxygen.recurse += doc
QMAKE_EXTRA_TARGETS += doxygen

lrelease.target = lrelease
lrelease.CONFIG = recursive
lrelease.recurse_target = lrelease
lrelease.recurse += sub_src
QMAKE_EXTRA_TARGETS += lrelease

DISTFILES += .qmake.conf \
	sync.profile
