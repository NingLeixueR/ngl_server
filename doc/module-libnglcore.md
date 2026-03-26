# libnglcore 模块

`server/runtime/libnglcore` 产出 `nglcore` 库，是服务端运行时的核心装配层。

## 核心职责

- 汇总 `public/cpp` 中的公共能力。
- 组织服务端启动链。
- 提供节点初始化与网络装配。
- 向 `node` 和测试目标暴露统一运行时入口。

## 关键目录

- `server/runtime/libnglcore/core`
  运行时主源码目录。
- `server/runtime/libnglcore/core/bootstrap`
  启动装配、节点初始化和进程入口相关源码。

当前重点文件包括：

- `server/runtime/libnglcore/core/bootstrap/server_main.cpp`
- `server/runtime/libnglcore/core/bootstrap/init_server.cpp`
- `server/runtime/libnglcore/core/bootstrap/start_node.h`
- `server/runtime/libnglcore/core/bootstrap/start_*.cpp`

## 主要下游

- `server/runtime/node`
- `server/tests`

## 维护建议

- 启动链逻辑尽量集中在 `core/` 内部，不向外层目录泄漏实现细节。
- 当前已经把 `core/` 中的启动装配代码移入 `bootstrap/`，后续可继续拆分 `runtime/` 和 `tooling/`。
