TEMPLATE = subdirs

SUBDIRS += \
	AutoUpdater \
	AutoUpdaterWidgets \
	Tests

DISTFILES += merge_libs_scripts/win/merge_libs.bat

AutoUpdaterWidgets.depends += AutoUpdater
Tests.depends += AutoUpdater AutoUpdaterWidgets
