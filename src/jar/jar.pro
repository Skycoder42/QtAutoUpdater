TARGET = QtAutoUpdaterCorePlayStorePlugin

load(qt_build_paths)
CONFIG += java

DESTDIR = $$MODULE_BASE_OUTDIR/jar

JAVACLASSPATH += $$PWD/src /home/sky/.gradle/caches/transforms-1/files-1.1/core-1.6.3.aar/6425ded82ae8acd2a1c94d6b9f0dc10b/jars/classes.jar
JAVASOURCES += $$PWD/src/de/skycoder42/qtautoupdater/core/plugin/qplaystore/UpdateHelper.java

# install
target.path = $$[QT_INSTALL_PREFIX]/jar
INSTALLS += target
