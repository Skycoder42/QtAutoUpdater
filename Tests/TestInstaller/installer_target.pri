installer.target = installer
win32: installer.commands = $$PWD/build_win.bat "C:\\Qt\\Tools\\QtInstallerFramework\\2.0" "$$PWD" "$$OUT_PWD/Tests/TestInstaller"
else:mac: installer.commands = $$PWD/build_mac.command "/home/sky/Qt/Tools/QtInstallerFramework/2.0" "$$PWD" "$$OUT_PWD/Tests/TestInstaller"
else: installer.commands = $$PWD/build_x11.sh "/home/sky/Qt/Tools/QtInstallerFramework/2.0" "$$PWD" "$$OUT_PWD/Tests/TestInstaller"

QMAKE_EXTRA_TARGETS += installer
