@echo off

:: builds
set PATH=C:\Python37-x64;C:\Python37-x64\Scripts;C:\projects\;%PATH%

echo %PLATFORM% | findstr /C:"mingw" > nul && (
	call %~dp0\appveyor_mingw.bat || exit /B 1
) || (
	:: prepare vcvarsall
	if "%APPVEYOR_BUILD_WORKER_IMAGE%" == "Visual Studio 2017" (
		set VC_DIR="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
	)

	:: find the varsall parameters
	if "%PLATFORM%" == "msvc2017_64" set VC_VARSALL=amd64
	if "%PLATFORM%" == "msvc2017" set VC_VARSALL=amd64_x86
	if "%PLATFORM%" == "winrt_x64_msvc2017" set VC_VARSALL=amd64_x86
	if "%PLATFORM%" == "winrt_x86_msvc2017" set VC_VARSALL=amd64_x86
	if "%PLATFORM%" == "winrt_armv7_msvc2017" set VC_VARSALL=amd64_x86

	:: build
	call %~dp0\appveyor_msvc.bat || exit /B 1
)
