protoc.exe --version
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro net.proto
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro db.proto
protoc.exe --experimental_allow_proto3_optional  --cpp_out=./pro example.proto
#protoc.exe --experimental_allow_proto3_optional --php_out=./pro/phpcode/db db.proto

protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro net.proto
protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro db.proto
protoc.exe --experimental_allow_proto3_optional  --csharp_out=./pro example.proto

xcopy pro\*.cc ..\..\public\cpp\protocol\protobuf /s/y
xcopy pro\*.h ..\..\public\cpp\protocol\protobuf /s/y
xcopy pro\*.cs ..\..\public\csharp\csharp\csharp\protobuf /s/y

#xcopy /E /I /Y pro\phpcode  ..\..\..\php\

xcopy ..\..\..\bin\Debug\makeproto.exe .\ /s/y
xcopy ..\..\..\bin\Release\makeproto.exe .\ /s/y

makeproto.exe ./

pause