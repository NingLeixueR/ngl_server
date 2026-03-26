# node 模块

`server/runtime/node` 负责把 `nglcore` 链接成最终的服务进程入口程序。

## 职责

- 提供服务端主进程入口。
- 安装崩溃转储与基础控制台行为。
- 调用 `ngl_main()` 进入运行时启动链。

## 关键入口

- `server/runtime/node/core/main.cpp`
- `server/runtime/node/core/dumper.h`

## 依赖关系

- 上游依赖：`server/runtime/libnglcore`
- 下游产物：`node.exe`
