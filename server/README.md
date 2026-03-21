# Server Directory Overview

`server` 目录主要负责服务端可执行程序、运行时核心库、工具生成器和测试。

## `./libnglcore`

- 服务端核心运行时库。
- 包含启动流程、参数解析、运行时辅助逻辑等。

## `./node`

- 服务端进程入口目标。
- 负责把运行时核心库组合成最终可执行程序。

## `./tests`

- GoogleTest 测试集。
- 覆盖序列化、运行时配置、网络、工具函数、性能基准等多个模块。

## `./idl`

- 基于 IDL 定义生成表结构和相关辅助代码。

## `./makeproto`

- 根据 `db.proto` 生成 `create_db.sql`。
- 根据 `net.proto` 生成协议 XML 与消息编号映射。

## `./rebuild`

- 历史维护工具目标。
- 当前 native unity build 已由 CMake 在构建目录中生成统一编译单元，不再依赖仓库内提交的 `ub_*.cpp`。
