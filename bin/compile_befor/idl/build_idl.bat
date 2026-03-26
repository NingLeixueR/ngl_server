@echo off
REM File overview: Automates the build idl workflow on Windows.

setlocal

set "lbat=%~dp0"
for %%I in ("%lbat%..\..\..") do set "lroot=%%~fI"
set "lbin=%lroot%\bin\Debug"
set "lsrc=%lbat%idlfile"
set "ltab=%lroot%\public\cpp\actor\tab"
set "lcsv=%lroot%\public\csharp\csharp\csharp\csv"

if not exist "%lbin%\IDL.exe" (
	echo Missing tool: "%lbin%\IDL.exe"
	exit /b 1
)

if not exist "%lsrc%" mkdir "%lsrc%"
if not exist "%ltab%" mkdir "%ltab%"
if not exist "%lcsv%" mkdir "%lcsv%"

pushd "%lbin%" || exit /b 1
IDL.exe idl || (
	popd
	exit /b 1
)
popd

if exist "%lsrc%\*.h" xcopy "%lsrc%\*.h" "%ltab%\" /i /y >nul || exit /b 1
if exist "%lsrc%\*.cpp" xcopy "%lsrc%\*.cpp" "%ltab%\" /i /y >nul || exit /b 1
if exist "%lsrc%\*.cs" xcopy "%lsrc%\*.cs" "%lcsv%\" /i /y >nul || exit /b 1
