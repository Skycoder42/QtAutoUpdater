#!/bin/bash
# IMPORTANT: Adjust path to script of https://github.com/Skycoder42/QtModules (repogen.py)
# $1 path to module binaries
# $2 Version

myDir=$(dirname "$0")
qtDir=${1?First parameter must be set to the dir to install}
version=${2?Set the version as second parameter}

"$myDir/../QtModules/deploy/repogen.py" "$qtDir" AutoUpdater "" "" "A Qt library to automatically check for updates and install them." "$version" "$myDir/LICENSE" BSD-3-Clause  "android_armv7,android_x86,ios,winrt_x86_msvc2017,winrt_x64_msvc2017,winrt_armv7_msvc2017"
