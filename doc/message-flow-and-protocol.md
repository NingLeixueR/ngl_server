# 消息流与协议系统

本文档说明 ngl_server 中消息的定义、注册、路由和分发机制。所有 Actor 间通信（无论本地还是跨节点）都经过此系统。

## 相关文档

- [Actor 系统架构](./actor-system-architecture.md) — Actor 生命周期与调度
- [网络层](./network-layer.md) — 传输层实现
- [网关架构](./gateway-architecture.md) — 客户端与服务端的消息桥接

## 协议注册：`tprotocol`

每种消息类型都有唯一的协议编号。`tprotocol`（`actor/protocol/tprotocol.h`）维护三个查找表：

- `m_keyval` — C++ 类型哈希 → `info` 结构体（协议编号、名称、优先级、脚本桥接）
- `m_protocol` — 协议编号 → `info*`
- `m_nameprotocol` — 名称字符串 → `info*`

注册方式有两种：
- **自定义 C++ 协议**：`tprotocol::tp_customs<T>()` 分配自增 ID
- **Protobuf 协议**：`tprotocol::init_protobufs<T>()` 从 `xmlprotocol` 元数据解析 ID

每个协议有 `m_highvalue` 字段决定调度优先级（值越小越紧急，0 为普通）。详见 [actor-ready-hightlevel.md](./actor-ready-hightlevel.md)。

## 消息信封：`handle_pram`

`handle_pram`（`actor/actor_base/core/handle_pram.h`）是类型擦除的消息载体：

| 字段 | 用途 |
|------|------|
| `m_enum` | 协议 ID（来自 `tprotocol`） |
| `m_data` | `shared_ptr<void>` — 类型化的消息负载 |
| `m_pack` | 转发二进制数据时的原始 `pack` |
| `m_actor` | 目标 Actor guid |
| `m_requestactor` | 发送方 Actor guid |
| `m_massactors` | 批量投递的目标集合 |
| `m_forward` | 远程转发的传输回调 |
| `m_forwardtype` | 为 true 时按 Actor 类型广播 |

`handle_pram::create<T>()` 从类型化负载构建 pram，自动解析协议编号并绑定传输回调。

## 处理函数注册：`nrfun<T>`

每种 Actor 类型有一个单例分发表 `nrfun<TDerived>`（`actor/actor_base/core/nrfun.h`）。处理函数在 Actor 的静态 `nregister()` 方法中注册：

| 方法 | 用途 |
|------|------|
| `rfun<TDerived, T>(handler)` | 网络可见的消息处理 |
| `rfun_nonet<TDerived, T>(handler)` | 仅本地处理（定时器、内部事件） |
| `rfun_c2g<T>(handler)` | 客户端到网关的转发处理 |
| `rfun_g2c<T>(handler)` | 网关到客户端的转发处理 |

`actor.h` 中的便捷封装：
- `register_handle<TDerived, Msg1, Msg2, ...>()` — 为每个消息类型注册 `TDerived::handle`
- `register_timer<TDerived>()` — 注册定时器回调
- `register_forward_c2g/g2c<TDerived, Msg...>()` — 网关转发
- `register_secondary_forward_c2g<TDerived, ACTOR, Msg...>()` — 二跳中继（如 role → chat）

## 类型化消息包装：`message<T>`

处理函数被调用时收到 `message<T>`，包含：
- `get_data()` — 反序列化后的消息负载指针
- `get_pack()` — 原始网络 `pack`（可获取 session id 等信息）
- 当前工作线程 ID

## 端到端消息流

### 客户端 → 游戏服务器（C2G）

```
客户端发送 protobuf 字节流
  → TCP/WS/KCP 传输层接收
  → segpack 重组为完整 pack
  → actor_gateway_c2g 反序列化，包装为 np_actor_forward<T, forward_c2g<forward>>
  → ntcp::send_server() 转发到玩家所在的游戏服务器
  → 游戏 Actor 的注册处理函数被调用
```

### 游戏服务器 → 客户端（G2C）

```
游戏 Actor 调用 send_client(actorid, data)
  → 包装为 np_actor_forward<T, forward_g2c<T>>
  → handle_pram 通过 gatewayid() 解析网关服务器
  → 发送到网关节点的 actor_gateway_g2c
  → actor_gateway_g2c 从 gateway_info 解析 socket
  → nnet::send() 序列化并写入客户端 socket
```

### Actor → Actor（内部通信）

```
actor_base::send_actor(guid, request_guid, data)
  → handle_pram::create<T>() 构建消息信封
  → actor_manage::push_task_id(guid, pram)
  → layer_index(guid) 选择 schedule_layer
  → 如果 Actor 在本地：直接入队
  → 如果 Actor 在远程：m_forward 回调通过 nnet 发送
```

### 批量投递

`send_actor(set<actorid>, data)` 按服务器 ID 分组目标，本地的直接投递，远程的按目标服务器批量打包为 `np_mass_actor<T>` 消息发送。

## 转发包装器

- `np_actor_forward<T, forward_c2g<forward>>` — 包装客户端消息用于网关 → 游戏服务器转发
- `np_actor_forward<T, forward_g2c<T>>` — 包装游戏响应用于游戏 → 网关 → 客户端投递
- `forward_c2g<forward>` 携带原始二进制；`forward_g2c<T>` 携带类型化数据 + 目标 uid/area 列表

### 二跳转发

部分客户端消息需要额外一跳。例如聊天消息的路径：
```
客户端 → gateway_c2g → actor_role → actor_chat
```
通过 `register_secondary_forward_c2g<actor_role, ACTOR_CHAT, PROBUFF_NET_CHAT>()` 注册。

## 关键文件

| 文件 | 职责 |
|------|------|
| `actor/protocol/tprotocol.h` | 协议注册表（ID、名称、优先级、脚本桥接） |
| `actor/actor_base/core/handle_pram.h` | 类型擦除的消息信封 |
| `actor/actor_base/core/nrfun.h` | 每种 Actor 类型的分发表 |
| `actor/actor_logic/nforward.h` | 转发注册入口 |
| `actor/actor_logic/nprotocol_c2g.cpp` | C2G 转发注册 |
| `actor/actor_logic/nprotocol_g2c.cpp` | G2C 转发注册 |
