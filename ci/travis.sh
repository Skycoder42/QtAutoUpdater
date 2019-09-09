#/!bin/bash
set -ex

if [ $PLATFORM == "gcc_64" ]; then
	mkdir pkgit && pushd pkgit 
	PATH=/opt/qt/$QT_VER/$PLATFORM/bin:$PATH ../src/3rdparty/build-pkgkit-qt.sh
	popd
	pkgconf --exists packagekitqt5
fi
