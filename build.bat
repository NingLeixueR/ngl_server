conan profile detect --force
#根据路径修改编译器版本 compiler.cppstd=20
#-c "tools.files.download:verify=False" 设置不进行ssl验证(确保网络环境安全)
#conan install . --output-folder=build/Release --build=missing -s build_type=Release -c "tools.files.download:verify=False" -s compiler.cppstd=20
#conan install . --output-folder=build/Debug --build=missing -s build_type=Debug -c "tools.files.download:verify=False"

conan install . -s build_type=Release -s compiler.cppstd=20
conan install . -s build_type=Debug -s compiler.cppstd=20
cmake --preset conan-default
pause