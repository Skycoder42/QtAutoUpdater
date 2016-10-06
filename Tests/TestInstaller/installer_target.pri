installer.target = installer
installer.commands = $$PWD/build_win.bat "C:\\Qt\\Tools\\QtInstallerFramework\\2.0" "$$PWD" "$$OUT_PWD/Tests/TestInstaller"

QMAKE_EXTRA_TARGETS += installer
