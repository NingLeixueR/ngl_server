# server 模块总览

`server` 是服务端主工作区，负责把公共基础库、运行时核心、工具和测试拼装成可执行工程。

## 目录职责

- `server/libnglcore`
  运行时核心库，负责启动、节点分派、运行时辅助和部分测试支撑代码。
- `server/node`
  最终服务进程入口，把 `nglcore` 组合成可执行程序。
- `server/tests`
  GoogleTest 测试集，覆盖运行时、网络、序列化、工具和性能基准。
- `server/idl`
  IDL 相关生成工具，可执行文件名为 `IDL`。
- `server/makeproto`
  协议和数据库相关生成工具，可执行文件名为 `makeproto`。
- `server/rebuild`
  本地 Windows 工具目标，当前主要是历史维护用途。

## CMake 装配关系

`server/CMakeLists.txt` 做的是“装配”而不是“实现”：

- 总是构建 `libnglcore`
- 总是构建 `node`
- 在 `BUILD_TESTING=ON` 时构建 `tests`
- 仅在本地 Windows 环境构建 `rebuild`、`makeproto`、`idl`

这意味着：

- CI 的主链路重点是 `nglcore + node + tests`
- 工具链模块默认不是跨平台 CI 的核心路径

## 典型依赖方向

```text
public/cpp
  └─ 被 libnglcore / tests / idl / makeproto 复用

libnglcore
  └─ 被 node 和 tests 依赖

node
  └─ 是最终服务入口

tests
  └─ 回归验证 libnglcore 与 public/cpp 行为
```

## 常见改动入口

- 修改启动流程
  先看 `server/libnglcore/core/server_main.cpp`
- 修改节点启动前校验
  先看 `server/libnglcore/core/startup_support.cpp`
- 修改进程入口或崩溃转储
  先看 `server/node/core/main.cpp`
- 修改测试注册或 CTest 标签
  先看 `server/tests/CMakeLists.txt`
- 修改工具程序生成逻辑
  看 `server/idl/core` 或 `server/makeproto/core`

## 维护建议

- 保持 `server/CMakeLists.txt` 只做装配，不把业务逻辑塞进去。
- 新增服务端模块时，优先明确它是“运行时核心”、“工具”还是“测试支撑”。
- 如果某个模块只在本地 Windows 使用，应在文档里显式标注，避免被误认为 CI 主链路的一部分。
