cd idl
call build_actor.bat
cd ..\compile_befor\idl
call build_idl.bat
cd ..\compile_befor\idl
call build_protocol.bat
cd ..\compile_befor\proto
call build_protocol.bat
cd ..\compile_befor\idl
call rebuild.bat
mkdir log
mkdir csv
xcopy ..\configure\csv\*.csv csv\ /s/y
mkdir config
xcopy ..\configure\config\*.xml config\ /s/y
mkdir csv\config 
xcopy ..\configure\config\config\*.xml config\config\ /s/y
xcopy ..\..\third_party\lib\*.dll ..\Debug\ /s/y
xcopy ..\configure\windows\*.bat ..\Debug\ /s/y

pause