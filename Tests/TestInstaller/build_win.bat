:: %1: path to Qt-Installer-Framwork
:: %2: $$PWD
:: %3: $$OUT_PWD
@echo off

:: create the installer (and online repo)

"%~1\bin\repogen.exe" --update-new-components -p %~2/packages %~3/QtAutoUpdaterTestInstaller
"%~1\bin\binarycreator.exe" -n -c %~2/config/config.xml -p %~2/packages %~3/QtAutoUpdaterTestInstaller.exe
