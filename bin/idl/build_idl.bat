xcopy ..\Debug\IDL.exe .\ /s/y
#xcopy ..\Release\IDL.exe .\ /s/y
IDL.exe idl
xcopy idlfile\*.h ..\..\public\cpp\protocol\ /s/y
xcopy idlfile\*.cpp ..\..\public\cpp\protocol\ /s/y
xcopy idlfile\*.cs ..\..\public\csharp\csharp\csharp\csv\ /s/y
pause

