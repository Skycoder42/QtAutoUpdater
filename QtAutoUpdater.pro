TEMPLATE = subdirs

SUBDIRS += \
	AutoUpdater \
    Tests

Tests.depends += AutoUpdater
