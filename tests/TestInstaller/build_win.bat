:: %1: $$PWD
:: working directory: $$OUT_PWD
:: @echo off

:: create the installer (and online repo)

repogen.exe --update-new-components -p "%~1/packages" ./QtAutoUpdaterTestInstaller
binarycreator.exe -n -c "%~1/config/config.xml" -p "%~1/packages" ./QtAutoUpdaterTestInstaller.exe
