TEMPLATE = lib
CONFIG += staticlib

QT = core autoupdatercore testlib autoupdatercore-private core-private

HEADERS += \
	plugintest.h

SOURCES += \
	plugintest.cpp

DISTFILES += \
	testlib.pri

runtarget.target = run-tests
!compat_test {
	win32: runtarget.depends += $(DESTDIR_TARGET)
	else: runtarget.depends += $(TARGET)
}
QMAKE_EXTRA_TARGETS += runtarget
