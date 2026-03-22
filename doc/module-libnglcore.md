# libnglcore 运行时核心

`server/libnglcore` 产出 `nglcore` 库，是服务端运行时的核心装配层。它站在 `public/cpp` 之上，把基础能力组织成可启动、可分派、可测试的运行时。

## 主要文件

`server/libnglcore/core` 当前重点文件包括：

- `server_main.cpp`
  节点启动主流程、节点分派、数据库初始化、机器人启动等。
- `server_main.h`
  `ngl_main()` 与 `startup_error` 等核心声明。
- `startup_support.cpp`
  启动前参数解析、配置加载、`tab_servers` 解析、失败日志。
- `startup_support.h`
  启动上下文与准备结果声明。
- `init_server.h`
  节点启动时的运行时装配辅助。
- `runtime_helpers.h`
  运行时辅助接口，例如机器人启动参数与 GM 推送参数构造。

## 模块职责

- 统一启动入口
- 将不同节点类型映射到各自启动路径
- 加载配置与 CSV 表
- 协调数据库、Actor、网络初始化顺序
- 向 `node` 模块暴露稳定入口

## 与其他模块的关系

- 上游依赖
  `public/cpp/actor`、`public/cpp/net`、`public/cpp/tools`
- 下游依赖
  `server/node`、`server/tests`

## 启动链路简图

```text
node/main.cpp
  -> ngl_main()
  -> startup_support::prep_ctx()
  -> 按 nodetype 分派 start_db/start_game/start_gateway/...
  -> init_server()
  -> 具体 actor 或服务模块注册
```

## 适合放在这里的代码

- 与“节点如何启动”直接相关的逻辑
- 与运行时配置加载顺序相关的逻辑
- 为多个节点共享的启动辅助函数

## 不适合放在这里的代码

- 具体业务 Actor 的领域实现
- 与某个工具程序强耦合的逻辑
- 纯通用工具函数

## 常见改动入口

- 新增节点类型
  改 `server_main.cpp` 的分派逻辑与对应初始化路径
- 调整启动参数解析
  改 `startup_support.cpp`
- 调整 GM 推送参数格式
  改 `runtime_helpers` 和 `server_main.cpp` 的 push 配置节点

## 维护建议

- 保持 `server_main.cpp` 聚焦装配，不把业务细节继续堆进去。
- 启动失败路径要尽量统一走 `startup_error` 和 `log_failure()`。
- 任何节点新增配置字段时，优先补测试再补实现。
