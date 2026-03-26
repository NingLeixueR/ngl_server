# idl 模块

`server/tools/idl` 是本地工具模块，用于基于 IDL 定义生成相关代码和辅助产物。

## 关键目录

- `server/tools/idl/core`

## 主要文件

- `server/tools/idl/core/idl.h`
- `server/tools/idl/core/idl.cpp`
- `server/tools/idl/core/idlcpp_protocol.h`
- `server/tools/idl/core/main.cpp`

## 定位

- 这是开发工具，不属于运行时主链路。
- 应该和 `runtime/` 目标隔离维护。
