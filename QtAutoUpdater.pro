TEMPLATE = subdirs

SUBDIRS += \
	UpdaterTest \
	AutoUpdater

UpdaterTest.depends += AutoUpdater
