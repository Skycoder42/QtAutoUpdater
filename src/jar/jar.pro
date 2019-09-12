TARGET = QtAutoUpdaterCorePlayStorePlugin

load(qt_build_paths)
CONFIG += java

DESTDIR = $$MODULE_BASE_OUTDIR/jar

JAVACLASSPATH += $$PWD/src
JAVASOURCES += $$PWD/src/de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper.java

# install
target.path = $$[QT_INSTALL_PREFIX]/jar
INSTALLS += target
