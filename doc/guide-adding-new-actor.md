# 新增 Actor 开发指南

本文档以 `actor_chat` 为例，手把手说明如何在 ngl_server 中新增一个 Actor。

## 前置阅读

- [Actor 系统架构](./actor-system-architecture.md) — 理解 Actor 生命周期和调度
- [消息流与协议系统](./message-flow-and-protocol.md) — 理解消息注册和分发
- [数据库层](./database-layer.md) — 如果需要持久化

## 文件结构

每个 Actor 遵循固定的三文件结构：

```
public/cpp/actor/actor_logic/actor_xxx/
  ├── actor_xxx.h                    类声明
  ├── actor_xxx.cpp                  构造、初始化、注册
  └── message/
      └── actor_xxx_handle.cpp       消息处理函数实现
```

## 第一步：声明类（`.h`）

以 `actor_chat` 为例（`actor/actor_logic/actor_chat/actor_chat.h`）：

```cpp
class actor_chat : public actor
{
    actor_chat(const actor_chat&) = delete;
    actor_chat& operator=(const actor_chat&) = delete;

    // 私有数据成员
    std::map<int64_t, int> m_lastspeakutc;

    actor_chat();  // 私有构造
public:
    friend class actor_instance<actor_chat>;  // 允许单例工厂访问
    static actor_chat& instance()
    {
        return actor_instance<actor_chat>::instance();
    }

    virtual ~actor_chat() = default;

    static ENUM_ACTOR actor_type();           // 返回 ACTOR_CHAT
    static i64_actorid actorid();             // 返回单例 Actor ID

    virtual void init();                       // 初始化（定时器、DB、订阅）
    virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);  // DB 加载完成回调

    static void nregister();                   // 注册消息处理函数

    bool timer_handle(const message<np_timerparm>& adata);  // 定时器回调
    bool handle(const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata);  // 消息处理
};
```

关键要素：
- 继承 `actor`
- 私有构造函数 + `friend class actor_instance<T>` 实现单例
- `actor_type()` 返回枚举值
- `actorid()` 返回 `nguid::make(TYPE, tab_self_area, nguid::none_actordataid())`
- `nregister()` 是静态方法，用于注册处理函数

## 第二步：实现构造和初始化（`.cpp`）

```cpp
actor_chat::actor_chat() :
    actor(actorparm
    {
        .m_parm
        {
            .m_type = ACTOR_CHAT,
            .m_area = tab_self_area
        },
        .m_weight = 0x7fffffff,  // 每次调度不限消息数
    })
{
}
```

`actorparm` 关键字段：
- `.m_parm.m_type` — Actor 类型枚举（必须）
- `.m_parm.m_area` — 分区 ID（通常为 `tab_self_area`）
- `.m_weight` — 每次调度最多处理的消息数
- `.m_timeout` — 单次调度的时间预算（毫秒）
- `.m_broadcast` — 是否参与广播 tick（有 DB 组件时自动开启）

### `init()` 方法

```cpp
void actor_chat::init()
{
    // 1. 设置定时器
    np_timerparm tparm;
    make_timerparm::make_interval(tparm, 2);  // 每 2 秒触发
    set_timer(tparm);

    // 2. 订阅其他 Actor 的数据（如 brief 摘要）
    std::set<i32_fieldnumber> lfieldset;
    tdb_brief::nsp_cread<actor_chat>::instance_readall(
        this, tools::pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase")
    );
}
```

### `nregister()` 方法

```cpp
void actor_chat::nregister()
{
    // 注册定时器回调
    actor::register_timer<actor_chat>(&actor_chat::timer_handle);

    // 注册消息处理函数
    register_handle<actor_chat,
        mforward<pbnet::PROBUFF_NET_CHAT>
    >();
}
```

注册方式说明：
- `register_timer<T>()` — 注册定时器
- `register_handle<T, Msg...>()` — 注册 `T::handle` 处理多种消息
- `register_actor<T>(handler...)` — 注册自定义处理函数
- `register_actornonet<T, Msg>(handler)` — 注册仅本地的处理函数

## 第三步：实现消息处理（`message/` 目录）

```cpp
bool actor_chat::handle(const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata)
{
    const auto* lpram = adata.get_data();
    // lpram->data() 获取 PROBUFF_NET_CHAT 消息体
    // lpram->identifier() 获取发送者 Actor ID
    // 处理逻辑...
    return true;
}
```

`mforward<T>` 是经过二跳转发的客户端消息包装器（客户端 → 网关 → actor_role → 目标 Actor）。

## 第四步：注册 Actor 类型

确保 `ENUM_ACTOR` 枚举中有新类型（如 `ACTOR_CHAT`），并在启动代码中实例化：

```cpp
actor_instance<actor_chat>::instance();
```

通常在 `server/libnglcore/core/bootstrap/start_game.cpp` 或类似的启动文件中。

## 第五步：注册协议转发（如需要）

如果 Actor 处理来自客户端的消息，需要在转发注册中添加：

```cpp
// nprotocol_c2g.cpp 中注册 C2G 转发
register_c2g<PROBUFF_NET_CHAT, ...>();

// 如果需要二跳转发（经过 actor_role）
register_c2g_2<ACTOR_CHAT, PROBUFF_NET_CHAT>();

// nprotocol_g2c.cpp 中注册 G2C 转发（如果有响应消息）
register_g2c<PROBUFF_NET_CHAT_RESPONSE, ...>();
```

## 添加数据库持久化（可选）

如果 Actor 需要持久化数据：

1. 在 `actorparm` 中设置 `.m_parm.m_manage_dbclient = true`
2. 声明 DB 组件成员（如 `ndb_modular<ENUM, TDATA, TACTOR>`）
3. 在 `init()` 中调用 `DB.set(this)` 绑定
4. 实现 `loaddb_finish()` 处理加载完成事件
5. 广播 tick 会自动刷盘

## 常见问题

| 问题 | 原因 |
|------|------|
| 消息收不到 | 忘记在 `nregister()` 中注册处理函数 |
| Actor 不启动 | 忘记在启动代码中调用 `actor_instance<T>::instance()` |
| DB 数据不保存 | `m_broadcast` 未开启（设置 `m_manage_dbclient = true` 会自动开启） |
| 客户端消息不到达 | 忘记在 `nprotocol_c2g.cpp` 中注册转发 |
| `actorparm` 断言失败 | `m_weight <= 0` 或 `m_timeout <= 0` |

## 关键文件

| 文件 | 职责 |
|------|------|
| `actor/actor_logic/actor_chat/actor_chat.h` | 示例 Actor 声明 |
| `actor/actor_logic/actor_chat/actor_chat.cpp` | 示例 Actor 实现 |
| `actor/actor_logic/actor_chat/message/actor_chat_handle.cpp` | 示例消息处理 |
| `actor/actor_logic/nprotocol_c2g.cpp` | C2G 转发注册 |
| `actor/actor_logic/nprotocol_g2c.cpp` | G2C 转发注册 |
