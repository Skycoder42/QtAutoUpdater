#!/bin/bash

scriptDir=$(dirname $(realpath $0))

sed -i 's/set(BUILD_SHARED_LIBS ON)/set(BUILD_SHARED_LIBS OFF CACHE BOOL "shared or static build")/g' "$scriptDir/PackageKit-Qt/CMakeLists.txt"
cmake \
	"-DCMAKE_INSTALL_PREFIX=$(qmake -query QT_INSTALL_PREFIX)" \
	"-DCMAKE_INSTALL_LIBDIR=$(qmake -query QT_INSTALL_LIBS)" \
	"-DCMAKE_INSTALL_INCLUDEDIR=$(qmake -query QT_INSTALL_HEADERS)" \
	"$scriptDir/PackageKit-Qt/"
make
make DESTDIR=$PWD/install install
