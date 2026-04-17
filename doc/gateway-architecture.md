# 网关架构

网关是外部客户端与内部 Actor 网格之间的桥梁。三个单例 Actor 协同工作，完成客户端消息的接入、转发和回传。

## 相关文档

- [Actor 系统架构](./actor-system-architecture.md) — Actor 生命周期与调度
- [消息流与协议系统](./message-flow-and-protocol.md) — 消息注册与分发
- [网络层](./network-layer.md) — 传输层实现

## 三个网关 Actor

| Actor | 文件 | 职责 |
|-------|------|------|
| `actor_gateway` | `actor/actor_logic/actor_gateway/actor_gateway.h` | 控制面：登录、会话管理、KCP 协商、断连处理 |
| `actor_gateway_c2g` | `actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h` | 数据面（入站）：反序列化客户端消息并转发到游戏服务器 |
| `actor_gateway_g2c` | `actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h` | 数据面（出站）：序列化游戏响应并发送给客户端 |

## 会话跟踪：`gateway_info`

`gateway_info`（`actor/actor_logic/actor_gateway/gateway_info.h`）维护两个映射：

- `m_info`：`area → dataid → gateway_socket` — 按分区和角色 ID 查找
- `m_sockinfo`：`socket → gateway_socket*` — 按 socket ID 查找

`gateway_socket` 结构体存储：
- `m_session` — 登录会话令牌
- `m_area` — 分区 ID
- `m_accountid` — 账号 ID
- `m_dataid` — 角色 ID
- `m_gameid` — 当前所在游戏服务器 ID
- `m_gatewayid` — 网关服务器 ID
- `m_socket` — TCP socket ID

关键方法：
- `updata(info)` — 新增或更新会话
- `updata_socket(area, dataid, socket)` — 绑定 socket 到已有会话
- `get(area, roleid)` / `get(socket)` — 查找会话
- `remove_socket(socket)` / `remove_actorid(actorid)` — 移除会话

## 登录流程

```
1. 客户端发送 PROBUFF_NET_ROLE_LOGIN（携带 roleid + session）
2. actor_gateway::handle() 验证 session 与 gateway_info 中的记录匹配
3. 如果已有旧 socket 连接：关闭旧连接，更新 socket 绑定
4. 绑定新 socket，更新 gateway_info
5. 将登录请求转发到玩家所在的游戏服务器（send_server）
6. 通过 update_gateway_info() 同步网关信息到其他相关 Actor
```

登录前置步骤（由 `actor_login` 完成）：
```
1. actor_login 验证账号凭证
2. 验证通过后发送 np_actorrole_login 给 actor_gateway
3. actor_gateway 在 gateway_info 中创建会话记录（socket = 0，等待客户端连接）
4. 通过 sync_actorserver_gatewayid() 通知 actor_server 建立 actorid → gateway 映射
```

## 断连处理

当 TCP 连接断开时：

```
1. 网络层产生 np_actor_session_close 消息
2. actor_gateway::handle() 接收
3. 如果是机器人测试模式：遍历所有匹配 socket 的会话
4. 否则：通过 gateway_info::get(socket) 查找会话
5. 调用 session_close(info) 处理断连逻辑
6. 通过 update_gateway_info() 广播删除信息
7. 从 gateway_info 中移除 socket 映射
```

`session_close()` 内部：
- 通知游戏服务器玩家断连（`np_actor_disconnect_close`）
- 通知登录服务器玩家断连
- 如果 socket 已为 0（未绑定连接），直接移除 actorid 映射

## KCP 会话协商

```
1. 客户端发送 PROBUFF_NET_KCPSESSION 到网关
2. actor_gateway 调用 ukcp::session_create() 生成会话密钥
3. 构建 np_actor_kcp 消息，包含：
   - 会话密钥、socket ID、客户端/服务端 Actor ID
   - 客户端 UDP 地址（IP + 端口）、conv、KCP 编号、服务器 ID
4. 发送给 actor_kcp（KCP 服务器上的 Actor）
5. actor_kcp 建立 KCP 端点
6. 网关收到 np_actor_kcp 响应后，构建 PROBUFF_NET_KCPSESSION_RESPONSE 发回客户端
```

## C2G 数据面：`actor_gateway_c2g`

`actor_gateway_c2g` 处理客户端到游戏服务器的消息转发：

1. 收到 `np_actor_forward<T, forward_c2g<forward>>` 包装的客户端消息
2. 从 `pack` 中提取 session ID
3. 通过 `gateway_info` 查找对应的 `gateway_socket`
4. 调用 `ntcp::send_server()` 转发到玩家所在的游戏服务器

在机器人测试模式下，通过 Actor ID 而非 socket ID 查找会话。

## G2C 数据面：`actor_gateway_g2c`

`actor_gateway_g2c` 处理游戏服务器到客户端的消息回传：

1. 收到 `np_actor_forward<T, forward_g2c<forward>>` 包装的游戏响应
2. 提取目标 UID/area 列表
3. 根据传输协议分支：
   - TCP/WS：通过 `gateway_info::get()` 解析 socket，调用 `nnet::send()`
   - KCP：通过 `nkcp::serkcp()` 解析 KCP 会话，调用 `sendbyactor()`

## 进程切换

当玩家需要切换游戏服务器时（如换线）：

1. 收到 `np_actorswitch_process<np_actorswitch_process_role>`
2. 更新 `gateway_socket::m_gameid` 为新的游戏服务器 ID
3. 通过 `update_gateway_info()` 广播更新

## 关键文件

| 文件 | 职责 |
|------|------|
| `actor/actor_logic/actor_gateway/actor_gateway.h` | 网关控制面 Actor |
| `actor/actor_logic/actor_gateway/gateway_info.h` | 会话跟踪数据结构 |
| `actor/actor_logic/actor_gateway/message/actor_gateway_handle.cpp` | 消息处理实现 |
| `actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h` | C2G 数据面 |
| `actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h` | G2C 数据面 |
