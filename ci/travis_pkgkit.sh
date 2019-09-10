#!/bin/bash
set -ex

apt-get -qq update
apt-get -qq install cmake packagekit

mkdir pkgit && pushd pkgit 

scriptDir=../src/3rdparty/PackageKit-Qt
sed -i 's/set(BUILD_SHARED_LIBS ON)/set(BUILD_SHARED_LIBS OFF)/g' "$scriptDir/CMakeLists.txt"

QMAKE=/opt/qt/$QT_VER/$PLATFORM/bin/qmake
export CMAKE_PREFIX_PATH=/opt/qt/$QT_VER/$PLATFORM:$CMAKE_PREFIX_PATH
cmake \
	"-DCMAKE_INSTALL_PREFIX=$($QMAKE -query QT_INSTALL_PREFIX)" \
	"-DCMAKE_INSTALL_LIBDIR=$($QMAKE -query QT_INSTALL_LIBS)" \
	"-DCMAKE_INSTALL_INCLUDEDIR=$($QMAKE -query QT_INSTALL_HEADERS)" \
	"$scriptDir"
make
make install

popd

export PKG_CONFIG_PATH=/opt/qt/$QT_VER/$PLATFORM/lib/pkgconfig/:$PKG_CONFIG_PATH
pkg-config --exists packagekitqt5
