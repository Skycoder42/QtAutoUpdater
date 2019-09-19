#!/bin/bash
set -ex

currDir=$(dirname $0)

# add packagekit-qt
if [[ $TRAVIS_OS_NAME == "linux" ]]; then
	# prepend pre build script
	mv qtmodules-travis/ci/linux/build-docker.sh qtmodules-travis/ci/linux/build-docker.sh.orig
	mv "$currDir/travis_qthttpserver.sh" qtmodules-travis/ci/linux/build-docker.sh
	if [[ $PLATFORM == "gcc_64" ]]; then
		cat "$currDir/travis_pkgkit.sh" >> qtmodules-travis/ci/linux/build-docker.sh
	fi
	cat qtmodules-travis/ci/linux/build-docker.sh.orig >> qtmodules-travis/ci/linux/build-docker.sh
else
	"$currDir/travis_qthttpserver.sh"
fi
