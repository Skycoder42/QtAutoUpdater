:: build
@echo off
setlocal

set qtplatform=%PLATFORM%
for %%* in (.) do set CurrDirName=%%~nx*

call %VC_DIR% %VC_VARSALL% || exit /B 1

set PATH=C:\Qt\Tools\QtCreator\bin\;%PATH%

mkdir build-qthttpserver
cd build-qthttpserver

C:\projects\Qt\%QT_VER%\%qtplatform%\bin\qmake ../src/3rdparty/qthttpserver || exit /B 1
jom || exit /B 1
jom install || exit /B 1

cd ..
