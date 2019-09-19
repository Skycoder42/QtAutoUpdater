#!/bin/bash
set -ex

mkdir qthttpserver && pushd qthttpserver 

/opt/qt/$QT_VER/$PLATFORM/bin/qmake ../src/3rdparty/qthttpserver
make
$SUDO make install

popd
