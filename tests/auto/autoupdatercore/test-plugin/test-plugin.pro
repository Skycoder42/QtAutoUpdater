TARGET = qtest

QT = autoupdatercore autoupdatercore-private

HEADERS += \
	qtestupdaterbackend.h \
	qtestupdaterplugin.h

SOURCES += \
	qtestupdaterbackend.cpp \
	qtestupdaterplugin.cpp

DISTFILES += \
	testplugin.json

PLUGIN_TYPE = updaters
PLUGIN_EXTENDS = autoupdatercore
PLUGIN_CLASS_NAME = QTestUpdaterPlugin
load(qt_plugin)

# disable installation
INSTALLS =
CONFIG -= create_cmake

runtarget.target = run-tests
!compat_test: runtarget.depends += $(DESTDIR_TARGET)
QMAKE_EXTRA_TARGETS += runtarget
