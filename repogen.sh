#!/bin/bash
# IMPORTANT: Adjust path to script of https://github.com/Skycoder42/QtModules (repogen.py)
# $1 path to module binaries
# $2 Version

myDir=$(dirname "$0")
"$myDir/../QtModules/repogen.py" "$1" AutoUpdater "A Qt library to automatically check for updates and install them." "$2" "$myDir/LICENSE" BSD-3-Clause "android_armv7,android_x86,ios,winrt_armv7_msvc2015,winrt_x64_msvc2015"