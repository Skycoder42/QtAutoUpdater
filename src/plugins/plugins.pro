TEMPLATE = subdirs

QT_FOR_CONFIG += core
SUBDIRS += \
	updater

# planned: flatpak, cask, playstore, iOs AppStore, "custom"
# future: snap
# custom: checks URI for updates using user-defined parser, can optionally download and execute something
#	-> test via python

# android: https://developer.android.com/guide/app-bundle/in-app-updates#java
