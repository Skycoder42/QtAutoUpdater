load(qt_parts)

SUBDIRS += doc

doxygen.target = doxygen
doxygen.CONFIG = recursive
doxygen.recurse_target = doxygen
doxygen.recurse += doc
QMAKE_EXTRA_TARGETS += doxygen

runtests.target = run-tests
runtests.CONFIG = recursive
runtests.recurse_target = run-tests
runtests.recurse += sub_tests sub_src
QMAKE_EXTRA_TARGETS += runtests

lupdate.target = lupdate
lupdate.CONFIG = recursive
lupdate.recurse_target = lupdate
lupdate.recurse += sub_src
QMAKE_EXTRA_TARGETS += lupdate

DISTFILES += .qmake.conf \
	sync.profile
