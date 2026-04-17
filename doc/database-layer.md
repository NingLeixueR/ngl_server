# 数据库层

ngl_server 的持久化抽象让 Actor 以组件化方式管理数据库表。数据修改通过脏标记跟踪，批量刷盘，支持 MySQL、PostgreSQL 和 Redis 后端。

## 相关文档

- [Actor 系统架构](./actor-system-architecture.md) — Actor 生命周期与广播 tick
- [新增 Actor 开发指南](./guide-adding-new-actor.md) — 如何为 Actor 添加数据库持久化

## 架构概览

```
Actor
  └── nmanage_dbclient          协调器：管理该 Actor 的所有 DB 客户端
        └── ndbclient<...>      每张表一个客户端实例
              └── data_modified<T>  行级脏标记跟踪
```

数据库操作通过专用的 DB Actor（`actor_db<DBTYPE, TDBTAB>`）在 DB 服务器节点上执行。业务 Actor 不直接访问数据库，而是通过消息与 DB Actor 通信。

## 核心组件

### `ndbclient_base` / `ndbclient<DBTYPE, TDBTAB, TACTOR>`

每张数据库表对应一个客户端实例（`actor/actor_base/core/ndbclient.h`）。

生命周期：
1. `init()` — 绑定到 `nmanage_dbclient` 和宿主 Actor
2. `load()` — 发送 `np_actordb_load` 请求到 DB Actor
3. 收到 `np_actordb_load_response`（可能分块，通过 `m_over` 标志判断是否完成）
4. `loadfinish()` — 通知 `nmanage_dbclient::on_load_finish()`
5. 运行时：脏数据通过 `savedb()` 批量保存，删除通过 `deldb()` 批量执行

关键接口：
- `isload()` — 是否已完成加载
- `create(guid)` — 创建新记录
- `savedb()` — 将脏行打包为 `np_actordb_save` 发送
- `deldb()` — 将待删除行打包为 `np_actordb_delete` 发送
- `clear_modified()` — 清除脏标记
- `nscript_push_data()` — 将数据同步到脚本运行时

### `data_modified<T>` 与 `nmodified<T>`

行级包装器，提供脏标记跟踪：
- `get()` — 可变访问（标记为脏，同步脚本编辑）
- `getconst()` — 只读访问
- `is_modified(id)` — 检查某行是否为脏
- `which_modified()` — 返回所有脏行 ID 集合

### `nmanage_dbclient`

每个 Actor 的数据库协调器（`actor/actor_base/core/nmanage_dbclient.h`）：
- `m_typedbclientmap` — 加载中的客户端
- `m_dbclientmap` — 已加载完成的客户端
- `on_load_finish()` — 将客户端从加载中提升到已加载，触发 Actor 的 `loaddb_finish()`
- `save()` — 刷新所有脏行
- `del()` — 刷新所有待删除行

### `ndb_component` 与 `ndb_modular<ENUM, TDATA, TACTOR>`

更高层的组件抽象（`actor/actor_base/core/ndb_modular.h`）：
- `set(actor_base*)` — 注册到宿主 Actor
- `init()` — 绑定 dbclient
- `create()` — 创建新记录
- `foreach()` / `find()` / `get()` / `erase()` — 数据访问
- `initdata()` — 虚函数，加载后的初始化钩子

## 保存/删除协议

- `np_actordb_save<DBTYPE, T>` — 批量脏行保存请求
- `np_actordb_delete<DBTYPE, T>` — 批量删除请求
- 两者都序列化为 `pack` 并通过 `send_actor()` 路由到 DB Actor

保存时机：
- 广播 tick 中 `handle_broadcast` 自动调用 `save()` 和 `del()`
- Actor 关闭时 `release()` 中同步刷盘

## 数据库后端

| 后端 | 管理类 | 连接池 |
|------|--------|--------|
| MySQL | `nmysql_manage` | `nmysql_pool` |
| PostgreSQL | `npostgresql_manage` | `npostgresql_pool` |
| Redis | `dbredis` | — |

## 关键文件

| 文件 | 职责 |
|------|------|
| `actor/actor_base/core/ndbclient.h` | 每表 DB 客户端（加载、保存、删除） |
| `actor/actor_base/core/nmanage_dbclient.h` | 每 Actor 的 DB 协调器 |
| `actor/actor_base/core/ndb_modular.h` | 高层组件抽象 |
| `tools/db/sql/mysql/nmysql.h` | MySQL 连接封装 |
| `tools/db/sql/db_data.h` | DB 数据结构定义 |
