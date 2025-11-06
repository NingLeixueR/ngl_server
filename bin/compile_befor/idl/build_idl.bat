cd ../../Debug/
IDL.exe idl
cd ../idl/
xcopy idlfile\*.h ..\..\..\public\cpp\actor\auto_edit\ /s/y
xcopy idlfile\*.cpp ..\..\..\public\cpp\actor\auto_edit\ /s/y
xcopy idlfile\*.cs ..\..\..\public\csharp\csharp\csharp\csv\ /s/y
pause