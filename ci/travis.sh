#/!bin/bash
set -ex

if [ $PLATFORM == "gcc_64" ]; then
	mkdir pkgit && pushd pkgit 
	
	scriptDir=../src/3rdparty/PackageKit-Qt
	sed -i 's/set(BUILD_SHARED_LIBS ON)/set(BUILD_SHARED_LIBS OFF)/g' "$scriptDir/CMakeLists.txt"

	export CMAKE_PREFIX_PATH=/opt/qt/$QT_VER/$PLATFORM:$CMAKE_PREFIX_PATH
	cmake $scriptDir
	make
	make install
	
	popd
	pkgconf --exists packagekitqt5
fi
