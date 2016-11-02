#-------------------------------------------------
#
# Project created by QtCreator 2015-12-21T17:36:25
#
#-------------------------------------------------

TEMPLATE = lib
QT -= gui
CONFIG += C++11

TARGET = QtAutoUpdaterCore
VERSION = 2.1.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtAutoUpdater Core Library"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "com.Skycoder42."
	QMAKE_FRAMEWORK_BUNDLE_NAME = "QtAutoUpdaterCore"

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''
} else:unix {
	QMAKE_LFLAGS += '-Wl,-rpath,\'.\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/../lib\''
	QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/lib\''
}

DEFINES += QTAUTOUPDATER_LIBRARY

include(../AutoUpdater/autoupdater.pri)

