@echo off
REM File overview: Automates the build protocol workflow on Windows.

setlocal

set "lbat=%~dp0"
for %%I in ("%lbat%..\..\..") do set "lroot=%%~fI"
set "lbin=%lroot%\bin\Debug"
set "lpro=%lbat%pro"
set "lprotoc=%lbat%protoc.exe"
set "lpb=%lroot%\public\cpp\actor\generated\pb"
set "lauto=%lroot%\public\cpp\actor\generated\auto"

if not exist "%lprotoc%" (
	echo Missing tool: "%lprotoc%"
	exit /b 1
)

if not exist "%lbin%\makeproto.exe" (
	echo Missing tool: "%lbin%\makeproto.exe"
	exit /b 1
)

if not exist "%lpro%" mkdir "%lpro%"
if not exist "%lpb%" mkdir "%lpb%"
if not exist "%lauto%" mkdir "%lauto%"

pushd "%lbat%" || exit /b 1
"%lprotoc%" --version || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --cpp_out="%lpro%" net.proto || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --cpp_out="%lpro%" db.proto || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --cpp_out="%lpro%" example.proto || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --csharp_out="%lpro%" net.proto || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --csharp_out="%lpro%" db.proto || (
	popd
	exit /b 1
)
"%lprotoc%" --experimental_allow_proto3_optional --csharp_out="%lpro%" example.proto || (
	popd
	exit /b 1
)
popd

if exist "%lpro%\*.cc" xcopy "%lpro%\*.cc" "%lpb%\" /i /y >nul || exit /b 1
if exist "%lpro%\*.h" xcopy "%lpro%\*.h" "%lpb%\" /i /y >nul || exit /b 1
REM xcopy "%lpro%\*.cs" "%lroot%\public\csharp\csharp\csharp\protobuf\" /i /y

pushd "%lbin%" || exit /b 1
makeproto.exe "%lbat%" || (
	popd
	exit /b 1
)
for %%I in (pb_protocol.cpp pbname_match.cpp nscript_pbdb.h nscript_pbexample.h nscript_pbnet.h) do (
	copy /y "%%I" "%lauto%\" >nul || (
		popd
		exit /b 1
	)
)
popd
