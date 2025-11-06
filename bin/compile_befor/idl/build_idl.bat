cd ../../Debug/
IDL.exe idl
cd ../compile_befor/idl/
xcopy idlfile\*.h ..\..\..\public\cpp\actor\tab\ /s/y
xcopy idlfile\*.cpp ..\..\..\public\cpp\actor\tab\ /s/y
xcopy idlfile\*.cs ..\..\..\public\csharp\csharp\csharp\csv\ /s/y
pause