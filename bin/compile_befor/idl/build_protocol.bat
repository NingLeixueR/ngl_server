REM File overview: Automates the build protocol workflow on Windows.

cd ../../Debug/
IDL.exe protocol
xcopy nprotocol_auto.h ..\..\public\cpp\actor\auto\ /s/y