@echo off
REM File overview: Builds ngl_server with the default Conan and CMake workflow.

setlocal

set "lbat=%~dp0"
for %%I in ("%lbat%..") do set "lbin=%%~fI\Debug"
for %%I in ("%lbat%..\configure") do set "lcfg=%%~fI"

call "%lbat%idl\build_actor.bat" || exit /b 1
call "%lbat%idl\build_idl.bat" || exit /b 1
call "%lbat%idl\build_protocol.bat" || exit /b 1
call "%lbat%proto\build_protocol.bat" || exit /b 1

if not exist "%lbin%\log" mkdir "%lbin%\log"
if not exist "%lbin%\csv" mkdir "%lbin%\csv"
if not exist "%lbin%\config" mkdir "%lbin%\config"
if not exist "%lbin%\config\config" mkdir "%lbin%\config\config"

if exist "%lcfg%\csv\*.csv" xcopy "%lcfg%\csv\*.csv" "%lbin%\csv\" /i /y >nul || exit /b 1
if exist "%lcfg%\config\*.xml" xcopy "%lcfg%\config\*.xml" "%lbin%\config\" /i /y >nul || exit /b 1
if exist "%lcfg%\config\config\*.xml" xcopy "%lcfg%\config\config\*.xml" "%lbin%\config\config\" /i /y >nul || exit /b 1
if exist "%lcfg%\windows\*.bat" xcopy "%lcfg%\windows\*.bat" "%lbin%\" /i /y >nul || exit /b 1

pause
