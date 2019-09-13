TEMPLATE = app

QT += quick autoupdatercore

defineReplace(droidVersionCode) {
	segments = $$split(1, ".")
	for (segment, segments): vCode = "$$first(vCode)$$format_number($$segment, width=3 zeropad)"

	contains(ANDROID_TARGET_ARCH, arm64-v8a): \
		suffix = 0
	else:contains(ANDROID_TARGET_ARCH, x86_64): \
		suffix = 1
	else:contains(ANDROID_TARGET_ARCH, armeabi-v7a): \
		suffix = 2
	else:contains(ANDROID_TARGET_ARCH, x86): \
		suffix = 3
	else: \
		error(Unsupported ANDROID_TARGET_ARCH: $$ANDROID_TARGET_ARCH)
	# add more cases as needed

	return($$first(vCode)$$first(suffix))
}

TARGET = AndroidDemo
VERSION = 1.0.0
ANDROID_VERSION_NAME = $$VERSION
ANDROID_VERSION_CODE = $$droidVersionCode($$ANDROID_VERSION_NAME)

SOURCES += \
	main.cpp

RESOURCES += qml.qrc

ANDROID_PACKAGE_SOURCE_DIR = \
	$$PWD/android

DISTFILES += \
	android/AndroidManifest.xml \
	android/build.gradle \
	android/res/values/libs.xml

!android_test_build: target.path = $$[QT_INSTALL_EXAMPLES]/autoupdaterquick/AndroidDemo
!install_ok: INSTALLS += target

android_test_build {
	ANDROID_EXTRA_LIBS += $$(QMAKEPATH)/lib/libQt5AutoUpdaterCore.so
	ANDROID_EXTRA_LIBS += $$(QMAKEPATH)/plugins/updaters/libqplaystore.so
	QML_IMPORT_PATH += $$(QMAKEPATH)/qml
}
