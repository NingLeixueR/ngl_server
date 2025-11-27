mkdir idlfile
cd ..\..\Debug\
IDL.exe idl
xcopy ..\compile_befor\idl\idlfile\*.h ..\..\public\cpp\actor\tab\ /s/y
xcopy ..\compile_befor\idl\idlfile\*.cpp ..\..\public\cpp\actor\tab\ /s/y
xcopy ..\compile_befor\idl\idlfile\*.cs ..\..\public\csharp\csharp\csharp\csv\ /s/y