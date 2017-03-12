TEMPLATE = aux

OTHER_FILES += \
	build_mac.command \
	build_x11.sh \
	config/config.xml \
	packages/de.skycoder42.QtAutoUpdaterTestInstaller/meta/package.xml \
	packages/de.skycoder42.QtAutoUpdaterTestInstaller/data/version.txt \
	build_win.bat

installer.target = installer
win32: installer.commands = $$PWD/build_win.bat "$$PWD"
else:mac: installer.commands = $$PWD/build_mac.command "$$PWD"
else: installer.commands = $$PWD/build_x11.sh "$$PWD"

QMAKE_EXTRA_TARGETS += installer
