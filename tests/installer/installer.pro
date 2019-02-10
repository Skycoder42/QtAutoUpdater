TEMPLATE = aux

DISTFILES += \
	config/config.xml \
	packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml \
	config/controller.qs

QMAKE_EXTRA_TARGETS += run-tests
