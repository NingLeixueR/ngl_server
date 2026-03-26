# server 模块总览

`server` 是服务端工程入口层，负责把运行时、工具和测试装配成具体目标。

## 目录职责

- `server/runtime/libnglcore`
  运行时核心库，承载启动链、节点初始化和运行时装配。
- `server/runtime/node`
  最终服务进程入口。
- `server/tests`
  GoogleTest 测试集合。
- `server/tools/idl`
  IDL 代码生成工具。
- `server/tools/makeproto`
  协议与数据库相关生成工具。
- `server/tools/rebuild`
  本地维护工具。

## CMake 装配关系

`server/CMakeLists.txt` 只做组合：

- 总是装配 `server/runtime`
- 在本地 Windows 环境装配 `server/tools`
- 在 `BUILD_TESTING=ON` 时装配 `server/tests`

## 典型依赖方向

```text
public/cpp
  └─ 被 runtime / tools / tests 复用

runtime/libnglcore
  └─ 被 runtime/node 和 tests 依赖

runtime/node
  └─ 最终服务进程入口

tests
  └─ 验证 runtime/libnglcore 与 public/cpp
```

## 常见改动入口

- 修改启动流程
  先看 `server/runtime/libnglcore/core/bootstrap/server_main.cpp`
- 修改节点初始化
  先看 `server/runtime/libnglcore/core/bootstrap/init_server.cpp`
- 修改进程入口
  先看 `server/runtime/node/core/main.cpp`
- 修改测试装配
  先看 `server/tests/CMakeLists.txt`
- 修改本地生成工具
  看 `server/tools/idl/core` 或 `server/tools/makeproto/core`
