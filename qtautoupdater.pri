contains(QT, widgets) {
	win32 {
		msvc {
			contains(QT_ARCH, x86_64) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdaterController
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterControllerd
			} else {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdaterController
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterControllerd
			}
		} else:win32-g++ {
			CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterControllerd
		}
	} else:mac {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdaterController_debug
		}
	} else:unix {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug/ -lQtAutoUpdaterController
		}
	}
} else {
	win32 {
		msvc {
			contains(QT_ARCH, x86_64) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdater
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterd
			} else {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdater
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterd
			}
		} else:win32-g++ {
			CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdater
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterd
		}
	} else:mac {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdater
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdater_debug
		}
	} else:unix {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release -lQtAutoUpdater
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug -lQtAutoUpdater
		}
	}
}

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
