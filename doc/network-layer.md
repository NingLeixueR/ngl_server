# 网络层

网络层在统一的 `nnet` 门面后提供三种传输实现。所有传输共享相同的二进制包格式，并接入 Actor 消息分发系统。

## 相关文档

- [消息流与协议系统](./message-flow-and-protocol.md) — 消息接收后的路由机制
- [网关架构](./gateway-architecture.md) — 外部客户端如何通过网关连接

## 传输概览

| 传输方式 | 类 | 协议枚举 | 使用场景 |
|----------|-----|----------|----------|
| TCP | `ntcp` | `ENET_TCP` | 服务器间通信、客户端到网关 |
| WebSocket | `nws` | `ENET_WS` | 浏览器客户端 |
| KCP/UDP | `ukcp` / `nkcp` | `ENET_KCP` | 低延迟游戏流量 |

## `nnet` 门面

`nnet`（`net/nnet.h`）是所有网络操作的单例入口，根据会话协议分发到对应传输：

```cpp
nnet::instance().send(session, data, actorid, requestactorid);
// 内部：switch(protocol(session)) { case ENET_TCP: ntcp... case ENET_WS: nws... }
```

关键方法：
- `send<Y, T>(session, data, actorid, requestactorid)` — 序列化并发送到单个会话
- `send_server<Y, T>(serverid, data, actorid, requestactorid)` — 解析服务器 → 会话，然后发送
- `send_pack(session, pack)` — 发送预构建的二进制包
- `pool()` — 访问共享的 `bpool` 用于 pack 分配
- `close(session)` / `close_net(session)` — 断开连接

## TCP 传输：`ntcp`

`ntcp`（`net/tcp/ntcp.h`）拥有：
- `asio_tcp` 后端（Boost.Asio 接受器 + 会话管理）
- `segpack` 实例向量（每个 socket 工作线程一个）
- 用于出站 pack 分配的 `bpool`

接收路径：原始字节通过 `socket_recv()` 到达 → 局域网检查 → 每线程 `segpack` 重组 → 完整 `pack` 进入协议分发系统。

发送路径：`send<Y, T>()` 通过 `net_pack<T>::npack()` 将负载序列化为 `pack`，然后写入 ASIO 会话。

连接管理：
- `init(port, threadcount, outernet)` — 开始监听
- `connect(ip, port, callback, await, reconnect)` — 出站连接，支持自动重连
- `connect(serverid, callback, await, reconnect)` — 从配置解析服务器地址后连接
- `set_close(session, ip, port, callback)` — 注册断连时的重连行为

## WebSocket 传输：`nws`

`nws`（`net/tcp/ws/nws.h`）与 `ntcp` 共享相同的 `asio_tcp` 基础，但增加了 WebSocket 帧封装。提供相同的 `send<Y, T>()` 接口。用于无法使用原始 TCP 的浏览器客户端。

## KCP/UDP 传输

分为两层：
- `ukcp`（`net/udp/kcp/ukcp.h`）— 高层门面，基于 Actor 的路由和会话创建
- `nkcp`（`net/udp/kcp/nkcp.h`）— 每连接的 KCP 状态管理

KCP 会话协商流程：
1. 客户端向网关发送 `PROBUFF_NET_KCPSESSION`
2. 网关调用 `ukcp::session_create()` 生成会话密钥
3. 网关将 `np_actor_kcp` 转发给 KCP 服务器上的 `actor_kcp`
4. `actor_kcp` 建立 KCP 端点并回复 `PROBUFF_NET_KCPSESSION_RESPONSE`
5. 客户端使用会话密钥建立 UDP/KCP 通道

## 包重组：`segpack`

`segpack`（`tools/serialize/segpack.h`）处理 TCP 流的分片问题。每个 socket 工作线程有自己的 `segpack` 实例以避免竞争。

每个 socket 跟踪部分接收的包，直到头部和包体都完整：
- `push(socket, buffer, length, islan)` — 输入原始字节，消费或缓存后返回
- `close(socket)` — 断连时清理部分状态

特殊处理：
- 心跳包（`segpack_heartbeat::is_heartbeat()`）在本地消费，不进入协议队列
- Telnet 命令被内联检测和处理（`telnet_cmd()`）

## 二进制包格式：`pack`

`pack`（`tools/serialize/pack.h`）表示一个网络消息：

| 字段 | 用途 |
|------|------|
| `m_protocol` | 传输类型（`ENET_TCP`、`ENET_WS`、`ENET_KCP`） |
| `m_id` | 会话 ID |
| `m_head` | `pack_head` — 协议编号、Actor ID、包体长度 |
| `m_buff` / `m_len` | 包体缓冲区（池分配或内联 `m_auto`） |
| `m_bpool` | 指向分配池的反向指针，用于释放 |

Pack 分配使用 `bpool` — 共享网络缓冲池的薄封装。`pack::make_pack(pool, len)` 创建带预分配包体的新 pack。

可选特性（由 `sysconfig` 控制）：
- XOR 加密（`sysconfig::isxor()`）— 包体的简单混淆
- Varint 编码（`sysconfig::varint()`）— 紧凑整数表示

## 限速：`nrate`

`nrate`（`tools/serialize/nrate.h`）跟踪每个 socket 的消息速率。当某个 socket 在 `sysconfig::rate_interval()` 秒内超过 `sysconfig::rate_count()` 条消息时，连接会被标记或关闭。

限速计数在每个完整包完成时进行（而非每个 TCP 分片），通过 `pack::m_rate_accounted` 跟踪。

## 会话管理：`server_session`

`server_session`（`net/server_session.h`）将逻辑服务器 ID 映射到活跃的 TCP 会话 ID。`send_server()` 用它来解析跨节点消息的发送目标。

## 关键文件

| 文件 | 职责 |
|------|------|
| `net/nnet.h` | 统一传输门面 |
| `net/tcp/ntcp.h` | TCP 传输（ASIO 后端） |
| `net/tcp/ws/nws.h` | WebSocket 传输 |
| `net/udp/kcp/ukcp.h` | KCP/UDP 高层门面 |
| `net/udp/kcp/nkcp.h` | KCP 会话管理 |
| `tools/serialize/segpack.h` | TCP 包重组 |
| `tools/serialize/pack.h` | 二进制包结构 |
| `tools/serialize/nrate.h` | 每 socket 限速 |
| `net/server_session.h` | 服务器 ID → 会话 ID 映射 |
