# node 可执行入口

`server/node` 负责把 `nglcore` 链接成最终的服务进程入口程序。

## 主要文件

- `core/main.cpp`
  入口函数，处理控制台编码、参数检查、dumper 初始化并调用 `ngl_main()`
- `core/dumper.h`
  转储相关声明
- `core/gdbdmp.cpp`
  Linux 或 gdb 场景下的转储支撑
- `core/mdump.cpp`
  Windows minidump 相关实现

## 模块职责

- 提供进程级 `main()`
- 做最外层的启动前校验
- 安装异常转储和崩溃转储逻辑
- 把控制权交给 `nglcore`

## 设计原则

`node` 应尽量薄：

- 参数够不够
- 是否设置控制台环境
- 是否初始化 dump handler
- 调 `ngl_main()`

真正的运行时装配和业务初始化，不应继续堆进 `node`。

## 依赖关系

```text
node
  -> nglcore
  -> public/cpp/tools 中的部分基础设施
```

## 常见改动入口

- 修改崩溃转储策略
  看 `gdbdmp.cpp` / `mdump.cpp`
- 修改入口参数前置检查
  看 `main.cpp`
- 想新增节点类型
  不在这里做，应该去 `libnglcore`

## 维护建议

- 保持 `main.cpp` 尽量短。
- 入口层只处理进程级问题，不处理业务节点分派。
- Windows 和 Linux 的 dump 行为差异应继续封装在各自文件中，不要交叉污染。
