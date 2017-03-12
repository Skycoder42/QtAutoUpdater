#!/bin/bash
# $1: $$PWD
# working directory: $$OUT_PWD

#create the installer (and online repo)
repogen --update-new-components -p "$1/packages" ./QtAutoUpdaterTestInstaller
binarycreator -n -c "$1/config/config.xml" -p "$1/packages" ./QtAutoUpdaterTestInstaller.run
