# Actor 系统架构

Actor 模型是 ngl_server 的核心。所有游戏逻辑（登录、聊天、邮件、排行榜、网关路由等）都运行在 Actor 内部。Actor 之间通过异步消息通信，不共享可变状态。

## 相关文档

- [Actor 就绪与高优先级调度](./actor-ready-hightlevel.md) — 优先级门控机制详解
- [消息流与协议系统](./message-flow-and-protocol.md) — 消息注册、序列化与分发
- [新增 Actor 开发指南](./guide-adding-new-actor.md) — 手把手教程

## Actor 标识：`nguid`

每个 Actor 拥有一个全局唯一的 64 位 ID，封装在 `union nguid`（`actor/actor_base/core/nguid.h`）中：

```
| 16 位: type | 16 位: area | 32 位: dataid |
```

- **type**（`ENUM_ACTOR`）：Actor 类型，如 `ACTOR_ROLE`、`ACTOR_CHAT`、`ACTOR_GATEWAY` 等
- **area**（`i16_area`）：分区/分服 ID
- **dataid**（`i32_actordataid`）：实例 ID（如角色 Actor 的玩家 ID）

任意字段值为 `-1` 表示"无"或"全部"（用于广播寻址）。

关键工厂方法：
- `nguid::make(type, area, dataid)` — 完整指定
- `nguid::make(type)` — 单例寻址（area 和 dataid 为哨兵值）
- `nguid::moreactor(type)` — 广播给某类型的所有 Actor
- `nguid::make()` — 无效/空 ID

## 类继承关系

```
actor_base                          (actor/actor_base/core/actor_base.h)
  └── actor                         (actor/actor_base/core/actor.h)
        └── 具体 actor（如 actor_chat、actor_gateway 等）
```

### `actor_base`

抽象基类，提供：
- 身份标识（`id_guid()`、`nconfig`）
- 生命周期钩子：`init()`、`release()`、`save()`、`erase_actor_before()`
- 消息发送：`send_actor()`、`send_server()`、`send_client()`
- 数据库组件管理：`manage_dbclient()`、`ndb_component`
- 脚本运行时绑定
- 定时器注册

### `actor`

具体调度单元，在基类之上增加：
- **两个消息队列**（由 `std::shared_mutex` 保护）：
  - `m_list` — 普通优先级 FIFO 队列
  - `m_hightlist` — 高优先级分桶 map（key 越小越紧急）
- **调度参数**（通过 `actorparm`）：
  - `m_weight` — 每次调度切片最多处理的普通消息数
  - `m_timeout` — 单次 `actor_handle()` 的软时间预算（毫秒）
  - `m_broadcast` — 是否参与共享广播 tick
- **分发表**（`m_actorfun`）：由 `init_rfun<T>()` 注册的类型擦除 `nrfunbase*`
- **队列溢出告警**：积压超阈值时限频告警

## Actor 生命周期

Actor 依次经历以下状态（`actor_stat`，定义在 `actor_base.h`）：

```
actor_stat_init  →  actor_stat_free  →  actor_stat_list  →  actor_stat_run  →  actor_stat_close
    （创建）          （已注册）           （已入队）           （执行中）          （关闭中）
```

单例 Actor 通过 `actor_instance<T>::instance()`（`actor_manage.h`）创建：
1. 构造静态 `T` 实例
2. 调用 `actor_manage::add_actor()` 注册到对应的 `schedule_layer`
3. 状态转为 `actor_stat_free`，调用 `init()` 和 `init_db_component()`
4. 调用 `init_rfun<T>()` 绑定分发表
5. 调用 `T::nregister()` 注册消息处理函数

## 调度：`actor_manage` 与 `schedule_layer`

### 分层架构

`actor_manage`（`actor/actor_base/core/actor_manage.h`）是全局单例，拥有：
- **8 个 `schedule_layer` 实例** — 独立调度单元，各自拥有互斥锁、调度线程和 Actor 查找表
- **共享工作线程池** — `nthread` 实例，负责处理 Actor 消息批次

分层路由使用位掩码（`layer_index()`）：
- **单例 Actor**（type >= `ACTOR_SIGNLE_START`）：`type & 7`
- **动态 Actor**（如 `ACTOR_ROLE`）：`dataid & 7`

这样同类型的动态 Actor 会分散到不同层，而不是集中在一个层上。

### 调度循环

1. 消息通过 `push_task_id()` 到达 → 路由到对应层
2. 层将消息入队到 Actor 的队列（`push()`）
3. 如果 Actor 处于空闲状态，移入层的就绪队列（`m_actorlist`）
4. 层的调度线程唤醒，调用 `pop_free_hreads()` 获取工作线程
5. 工作线程调用 `actor_handle()`：
   - **阶段 1**：排空高优先级消息（将 `m_hightlist` swap 到本地副本）
   - **阶段 2**：排空普通消息，受 `m_weight` 数量和 `m_timeout` 时间预算限制
6. 工作线程通过 `push_workthreads()` 归还自身；如果 Actor 仍有待处理消息，重新进入就绪队列

### 挂起机制

`actor_suspend` 是一个 RAII 辅助类，用于暂停所有调度。`statrt_suspend_thread()` 自旋等待所有工作线程空闲；`finish_suspend_thread()` 释放它们。用于 CSV 热更新等需要协调的操作。

## 广播 Tick

在 `actorparm` 中设置 `m_broadcast = true` 的 Actor 会收到周期性的 `np_actor_broadcast` 消息。处理函数（`actor::handle_broadcast`）先通过 `manage_dbclient()->save()` 和 `del()` 刷新数据库状态，然后调用虚函数 `broadcast()` 执行 Actor 特定的周期性工作。

## 关键文件

| 文件 | 职责 |
|------|------|
| `actor/actor_base/core/nguid.h` | 64 位打包 Actor 标识 |
| `actor/actor_base/core/actor_base.h` | 抽象基类：身份、消息、数据库、脚本 |
| `actor/actor_base/core/actor.h` | 具体 Actor：队列与调度 |
| `actor/actor_base/core/actor.cpp` | 队列排空、优先级分发、广播处理 |
| `actor/actor_base/core/actor_manage.h` | 分层路由、工作线程池、单例实例化 |
| `actor/actor_base/core/nthread.h` | 处理 Actor 消息批次的工作线程 |
