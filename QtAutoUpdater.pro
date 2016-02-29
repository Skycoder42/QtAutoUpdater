TEMPLATE = subdirs

SUBDIRS += \
	AutoUpdaterCore \
	AutoUpdaterGui \
	Tests

DISTFILES += README.md \
	LICENSE \
    doc/QtAutoUpdater.doxy

AutoUpdaterGui.depends += AutoUpdaterCore
Tests.depends += AutoUpdaterCore AutoUpdaterGui
