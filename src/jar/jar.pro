TARGET = QtAutoUpdaterCorePlayStorePlugin

load(qt_build_paths)
CONFIG += java

DESTDIR = $$MODULE_BASE_OUTDIR/jar

JAVACLASSPATH += $$PWD/src
JAVASOURCES += $$PWD/src/de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper.java

# fetch core library dependency
PLAY_CORE_URL = "https://dl.google.com/dl/android/maven2/com/google/android/play/core/$${PLAY_CORE_VERSION}/core-$${PLAY_CORE_VERSION}.aar"

get_core_target.target = core-$${PLAY_CORE_VERSION}.aar
get_core_target.commands = curl -Lo core-$${PLAY_CORE_VERSION}.aar $$shell_quote($$PLAY_CORE_URL)
unzip_core_target.target = core-$${PLAY_CORE_VERSION}/classes.jar
unzip_core_target.depends = $${get_core_target.target}
unzip_core_target.commands = unzip core-$${PLAY_CORE_VERSION}.aar -d core-$${PLAY_CORE_VERSION} && touch $${unzip_core_target.target}
QMAKE_EXTRA_TARGETS += get_core_target unzip_core_target

JAVACLASSPATH += "$$OUT_PWD/core-$${PLAY_CORE_VERSION}/classes.jar"

load(java)
javac.depends += $${unzip_core_target.target}

# install
target.path = $$[QT_INSTALL_PREFIX]/jar
INSTALLS += target
