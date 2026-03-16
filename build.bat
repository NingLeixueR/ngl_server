@echo off
REM File overview: Builds ngl_server with the default Conan and CMake workflow.

setlocal

conan profile detect --force

REM Defaultexecute Debug (and CMakeUserPresets.json consistent)
conan install . --output-folder=build/Debug --build=missing -s build_type=Debug -s compiler.cppstd=20
cmake --preset conan-default
cmake --build --preset conan-debug

REM Release, execute:
REM -C "tools.files.download:verify=False" set ssl ( )
REM -C abseil/*:shared=False -o protobuf/*:shared=False ifprotobuf local
REM conan install . --output-folder=build/Release --build=missing -s build_type=Release -s compiler.cppstd=20
REM cmake -S . -B build/Release -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=build/Release/conan_toolchain.cmake
REM cmake --build build/Release --config Release

pause