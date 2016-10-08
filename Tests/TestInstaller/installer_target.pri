installer.target = installer
win32: installer.commands = $$PWD/build_win.bat "C:\\Qt\\Tools\\QtInstallerFramework\\2.0" "$$PWD"
else:mac: installer.commands = $$PWD/build_mac.command "/home/sky/Qt/Tools/QtInstallerFramework/2.0" "$$PWD"
else: installer.commands = $$PWD/build_x11.sh "/home/sky/Qt/Tools/QtInstallerFramework/2.0" "$$PWD"

QMAKE_EXTRA_TARGETS += installer
