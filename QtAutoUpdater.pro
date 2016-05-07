TEMPLATE = subdirs

SUBDIRS += \
	AutoUpdaterCore \
	AutoUpdaterGui \
	Tests

DISTFILES += README.md \
	LICENSE \
    doc/QtAutoUpdater.doxy \
	qtautoupdater.pri

AutoUpdaterGui.depends += AutoUpdaterCore
Tests.depends += AutoUpdaterCore AutoUpdaterGui
