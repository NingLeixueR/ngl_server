# Server Directory Overview

`server` 现在按职责划成三层：

- `runtime/`
  运行时目标与核心库。
- `tools/`
  本地开发工具与代码生成工具。
- `tests/`
  GoogleTest 测试目标与测试用例。

## `./runtime/libnglcore`

- 服务端运行时核心库。
- 负责启动流程、节点初始化、运行时装配和部分测试辅助。

## `./runtime/node`

- 最终服务进程入口。
- 负责把 `nglcore` 组装成可执行程序。

## `./tools/idl`

- 基于 IDL 定义生成相关代码和辅助产物。

## `./tools/makeproto`

- 根据协议与数据库定义生成配套文件。

## `./tools/rebuild`

- 本地维护工具目标。
- 目前仍主要服务于 Windows 本地开发流程。

## `./tests`

- GoogleTest 测试集合。
- 覆盖运行时、网络、序列化、工具函数和性能基准等模块。
