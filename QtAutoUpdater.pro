TEMPLATE = subdirs

SUBDIRS += \
	AutoUpdater \
	AutoUpdaterWidgets \
	Tests

DISTFILES += README.md \
	LICENSE \
	doc/QtAutoUpdater.doxy

AutoUpdaterWidgets.depends += AutoUpdater
Tests.depends += AutoUpdater AutoUpdaterWidgets
