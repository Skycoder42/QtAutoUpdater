TEMPLATE = subdirs

qtConfig(process): SUBDIRS += qtifw
packagesExist(packagekitqt5): SUBDIRS += qpackagekit
win32:qtConfig(process): SUBDIRS += qchocolatey
