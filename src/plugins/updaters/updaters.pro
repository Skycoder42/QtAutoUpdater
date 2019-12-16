TEMPLATE = subdirs

SUBDIRS += qwebquery
qtConfig(process):!android: SUBDIRS += qtifw
packagesExist(packagekitqt5): SUBDIRS += qpackagekit
win32:qtConfig(process): SUBDIRS += qchocolatey
mac:!ios:qtConfig(process): SUBDIRS += qhomebrew
android: SUBDIRS += qplaystore
