win32 {
	msvc {
		contains(QT_ARCH, x86_64) {
			contains(QT, widgets) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdaterController
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterControllerd
				QT += winextras
				LIBS += -lAdvapi32 -lOle32
			}
			CONFIG(release, debug|release): LIBS += -L$$PWD/msvc64/release/ -lQtAutoUpdater
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc64/debug/ -lQtAutoUpdaterd
		} else {
			contains(QT, widgets) {
				CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdaterController
				else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterControllerd
				QT += winextras
				LIBS += -lAdvapi32 -lOle32
			}
			CONFIG(release, debug|release): LIBS += -L$$PWD/msvc/release/ -lQtAutoUpdater
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/msvc/debug/ -lQtAutoUpdaterd
		}
	} else:win32-g++ {
		contains(QT, widgets) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterControllerd
			QT += winextras
			LIBS += -lAdvapi32 -lOle32
		}
		CONFIG(release, debug|release): LIBS += -L$$PWD/mingw/release/ -lQtAutoUpdater
		else:CONFIG(debug, debug|release): LIBS += -L$$PWD/mingw/debug/ -lQtAutoUpdaterd
	}
} else:mac {
	contains(QT_ARCH, x86_64) {
		contains(QT, widgets) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdaterController_debug
			LIBS += -framework Security
		}
		CONFIG(release, debug|release): LIBS += -L$$PWD/clang64/release/ -lQtAutoUpdater
		else:CONFIG(debug, debug|release): LIBS += -L$$PWD/clang64/debug/ -lQtAutoUpdater_debug
	}
} else:unix {
	contains(QT_ARCH, x86_64) {
		contains(QT, widgets) {
			CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release/ -lQtAutoUpdaterController
			else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug/ -lQtAutoUpdaterController
			LIBS += -lutil
		}
		CONFIG(release, debug|release): LIBS += -L$$PWD/gcc64/release -lQtAutoUpdater
		else:CONFIG(debug, debug|release): LIBS += -L$$PWD/gcc64/debug -lQtAutoUpdater
	}
}

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
