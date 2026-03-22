# public/cpp 公共基础库

`public/cpp` 是仓库里最底层、复用度最高的 C++ 公共代码区。它既服务于服务端，也被测试、工具程序和部分生成链路复用。

## 子目录职责

- `actor`
  Actor 体系、协议、表结构映射、自动生成代码和业务基础设施。
- `net`
  网络层与会话层，包含 TCP、UDP、WebSocket、KCP 相关支撑。
- `tools`
  通用工具层，覆盖日志、配置、序列化、命令行、时间轮、数据库工具等。
- `third_party`
  仓库内嵌的第三方源码或头文件目录。

## actor 模块

`public/cpp/actor` 主要可再分成几层：

- `actor_base`
  Actor 基类、调度和基础通信模型。
- `actor_logic`
  业务 Actor 与领域逻辑实现。
- `protocol`
  协议常量、消息路由和跨模块约定。
- `tab`
  表配置结构和运行时查表能力。
- `pb`
  protobuf 产物与相关定义。
- `auto`
  自动生成代码。

适合放在 `actor` 的内容：

- 与 Actor 生命周期直接相关的代码
- 跨节点消息和协议常量
- 与角色、场景、邮件、排行榜等领域逻辑紧密绑定的基础设施

不适合放在 `actor` 的内容：

- 纯通用字符串处理
- 与 Actor 无关的独立网络封装
- 只服务构建工具的私有脚本逻辑

## net 模块

`public/cpp/net` 提供网络基础设施，主要负责：

- 传输层封装
- session 管理
- 包头与数据帧组织
- 服务器会话索引
- 与 Asio/KCP 相关的基础能力

常见关键文件：

- `asio_base.*`
- `nnet.*`
- `server_session.*`
- `net_pack.h`
- `node_pack.h`

## tools 模块

`public/cpp/tools` 是杂而不乱的基础工具层，范围很广，主要包括：

- `log`
  日志
- `serialize`
  序列化与封包辅助
- `tab`
  CSV/XML 读写与配置解析
- `db`
  数据库辅助
- `curl`
  HTTP/邮件等外部请求能力
- `ai`
  行为树等 AI 相关支撑
- 顶层工具
  `tools.*`、`arg_options.h`、`time_wheel.*`、`localtime.*`、`nfilterword.*`

## 依赖关系

`public/cpp` 是上游模块，典型依赖方向是：

```text
public/cpp
  ├─ libnglcore
  ├─ tests
  ├─ idl
  ├─ makeproto
  └─ 部分 client 协议对齐参考
```

## 常见改动入口

- 修改 Actor 系统
  从 `public/cpp/actor` 开始
- 修改网络协议封包或 session 行为
  从 `public/cpp/net` 开始
- 修改工具类、配置解析、时间轮、命令行等
  从 `public/cpp/tools` 开始

## 维护建议

- 控制 `tools` 模块继续无边界膨胀，优先按主题归类。
- `actor_logic` 中新增逻辑前，先确认能否复用现有 `actor_base` 或协议层能力。
- `third_party` 和生成目录应与手写逻辑保持清晰边界，不要混写。
