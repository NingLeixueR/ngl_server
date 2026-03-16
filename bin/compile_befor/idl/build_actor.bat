REM File overview: Automates the build actor workflow on Windows.

cd ../../Debug/
IDL.exe actor
xcopy auto_actor_enum.h ..\..\public\cpp\actor\auto\ /s/y