xcopy ..\Debug\IDL.exe .\ /s/y
#xcopy ..\Release\IDL.exe .\ /s/y
IDL.exe actor
xcopy auto_actor_enum.h ..\..\public\cpp\actor\ /s/y
pause