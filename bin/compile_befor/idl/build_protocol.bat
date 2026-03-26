@echo off
REM File overview: Automates the build protocol workflow on Windows.

setlocal

set "lbat=%~dp0"
for %%I in ("%lbat%..\..\..") do set "lroot=%%~fI"
set "lbin=%lroot%\bin\Debug"
set "lauto=%lroot%\public\cpp\actor\generated\auto"

if not exist "%lbin%\IDL.exe" (
	echo Missing tool: "%lbin%\IDL.exe"
	exit /b 1
)

if not exist "%lauto%" mkdir "%lauto%"

pushd "%lbin%" || exit /b 1
IDL.exe protocol || (
	popd
	exit /b 1
)
copy /y "nprotocol_auto.h" "%lauto%\" >nul || (
	popd
	exit /b 1
)
popd
