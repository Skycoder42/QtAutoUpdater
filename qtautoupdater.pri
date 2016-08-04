contains(QT, widgets) {
	win32 {
		msvc {
			contains(QT_ARCH, x86_64) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdaterGui
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterGuid
			} else {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdaterGui
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterGuid
			}
		} else:win32-g++ {
			CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdaterGui
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterGuid
		}
	} else:mac {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdaterGui
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdaterGui_debug
		}
	} else:unix {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release/ -lQtAutoUpdaterGui
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug/ -lQtAutoUpdaterGui
		}
	}
} else {
	win32 {
		msvc {
			contains(QT_ARCH, x86_64) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdaterCore
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterCored
			} else {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdaterCore
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterCored
			}
		} else:win32-g++ {
			CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdaterCore
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterCored
		}
	} else:mac {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdaterCore
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdaterCore_debug
		}
	} else:unix {
		contains(QT_ARCH, x86_64) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release -lQtAutoUpdaterCore
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug -lQtAutoUpdaterCore
		}
	}
}

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
