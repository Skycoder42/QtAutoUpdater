TEMPLATE = subdirs

QT_FOR_CONFIG += core
SUBDIRS += \
	updater

# planned: PackageKit, flatpak, chocolaty, homebrew, playstore, iOs AppStore, "custom"
# future: snap
# custom: checks URI for updates using user-defined parser, can optionally download and execute something
#	-> test via python
