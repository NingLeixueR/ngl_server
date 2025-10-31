protoc.exe --version
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro net.proto
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro db.proto
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro example.proto
#protoc.exe --experimental_allow_proto3_optional --php_out=./pro/phpcode/db db.proto

protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro net.proto
protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro db.proto
protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro example.proto

xcopy pro\*.cc ..\..\public\cpp\actor\pb\ /s/y
xcopy pro\*.h ..\..\public\cpp\actor\pb\ /s/y
xcopy pro\*.cs ..\..\public\csharp\csharp\csharp\protobuf /s/y

#xcopy /E /I /Y pro\phpcode  ..\..\..\php\

#xcopy ..\Debug\makeproto.exe .\ /s/y
#xcopy ..\..\..\bin\Release\makeproto.exe .\ /s/y

cd ../Debug/
makeproto.exe ../proto
xcopy pb_protocol.cpp ..\..\public\cpp\actor\auto_edit\ /s/y
xcopy pbname_match.cpp ..\..\public\cpp\actor\auto_edit\ /s/y
xcopy nscript_pbdb.h ..\..\public\cpp\actor\auto_edit\ /s/y
xcopy nscript_pbexample.h ..\..\public\cpp\actor\auto_edit\ /s/y
xcopy nscript_pbnet.h ..\..\public\cpp\actor\auto_edit\ /s/y
pause