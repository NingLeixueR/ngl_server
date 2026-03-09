@echo off
setlocal

conan profile detect --force

REM 默认执行 Debug 构建链路（与 CMakeUserPresets.json 保持一致）
conan install . --output-folder=build/Debug --build=missing -s build_type=Debug -s compiler.cppstd=20
cmake --preset conan-default
cmake --build --preset conan-debug

REM 如需 Release，可手动执行：
REM -c "tools.files.download:verify=False" 设置不进行ssl验证(确保网络环境安全)
REM -c abseil/*:shared=False -o protobuf/*:shared=False 如果protobuf库本地编译失败可尝试
REM conan install . --output-folder=build/Release --build=missing -s build_type=Release -s compiler.cppstd=20
REM cmake -S . -B build/Release -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=build/Release/conan_toolchain.cmake
REM cmake --build build/Release --config Release

pause
