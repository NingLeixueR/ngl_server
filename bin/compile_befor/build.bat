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
pause