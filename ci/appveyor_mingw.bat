setlocal
@echo on

for %%* in (.) do set CurrDirName=%%~nx*

if "%PLATFORM%" == "mingw73_64" set MINGW_ARCH=64
if "%PLATFORM%" == "mingw73_32" set MINGW_ARCH=32

set PATH=C:\projects\Qt\Tools\mingw730_%MINGW_ARCH%\bin;%PATH%;
set MAKEFLAGS=-j%NUMBER_OF_PROCESSORS%

mkdir build-qthttpserver
cd build-qthttpserver

C:\projects\Qt\%QT_VER%\%PLATFORM%\bin\qmake ../src/3rdparty/qthttpserver || exit /B 1
mingw32-make || exit /B 1
mingw32-make install || exit /B 1

cd ..
