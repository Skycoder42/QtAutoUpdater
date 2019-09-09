#/!bin/bash
set -ex

apt-get -qq update
apt-get -qq install cmake packagekit pkgconf

mkdir pkgit && cd pkgit 

scriptDir=../src/3rdparty/PackageKit-Qt
sed -i 's/set(BUILD_SHARED_LIBS ON)/set(BUILD_SHARED_LIBS OFF)/g' "$scriptDir/CMakeLists.txt"

export CMAKE_PREFIX_PATH=/opt/qt/$QT_VER/$PLATFORM:$CMAKE_PREFIX_PATH
cmake $scriptDir
make
make install

cd ..
pkgconf --exists packagekitqt5
