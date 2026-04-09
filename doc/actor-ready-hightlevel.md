# Actor Ready 与 Hight 特权机制说明

本文档用于说明当前 `actor` 框架中 `ready` 与 `hight` 特权消息的设计、执行流程、语义约定与维护注意事项。

本文档基于当前代码实现整理，重点对应以下文件：

- `public/cpp/actor/actor_base/core/actor_base.h`
- `public/cpp/actor/actor_base/core/actor_base.cpp`
- `public/cpp/actor/actor_base/core/actor.cpp`
- `public/cpp/actor/protocol/nactor_auto.cpp`
- `public/cpp/actor/generated/auto/register_channel_db.h`
- `public/cpp/actor/actor_logic/actor_login/actor_login.cpp`

## 1. 设计目的

`ready` 机制用于描述 actor 在运行早期或运行中的“阶段性可用状态”。

当前设计目标不是简单区分“能处理消息”和“不能处理消息”，而是将消息划分为两类：

- 普通消息：依赖 `ready().is_ready()`，只有 actor 完全 ready 后才处理。
- 特权消息：用于推进 actor 进入 ready 状态，或者在 ready 尚未完成前执行必要的控制流。

因此，`hight` 特权消息不是普通优先级队列的简单加速器，而是 `ready` 状态推进机制的一部分。

## 2. 当前核心语义

### 2.1 `e_hightlevel` 的含义

定义位置：`public/cpp/actor/actor_base/core/actor_base.h`

当前枚举：

```cpp
enum e_hightlevel
{
	e_hightlevel_db = 1,
	e_hightlevel_nsp = 2,
	e_hightlevel_custom = 10,
};
```

这里的枚举值表示：

- 哪一类协议属于哪个特权等级。
- 哪一类 `ready` 条件由哪个特权等级的消息负责推进。

注意：

- 当前实现下，`ready` 条件本身并不直接存储在 `e_hightlevel` 对应的 key 上。
- `ready` 条件存储时会使用 `ahightlevel + 1` 作为内部 key。

也就是说，当前语义不是“key 本身就是协议等级”，而是：

- `协议高特权等级 = e_hightlevel`
- `ready 阶段内部 key = e_hightlevel + 1`

这样做的目的，是让“负责推进某阶段 ready 的消息”落在“该阶段 ready 条件”之前。

### 2.2 `nready::set_ready()` 的当前行为

定义位置：`public/cpp/actor/actor_base/core/actor_base.h`

当前实现：

```cpp
template <typename ...ARGS>
void set_ready(const std::string& akey, const std::function<bool()>& afun, e_hightlevel ahightlevel)
{
	auto& lvec = m_ready[ahightlevel + 1];
	lvec.emplace_back(...);
	if constexpr (sizeof...(ARGS) > 0)
	{
		tprotocol::set_hightvalue<ARGS...>(ahightlevel);
	}
}
```

这段代码有两个效果：

1. 注册一个 ready 条件。
   这个条件会被放进 `m_ready[ahightlevel + 1]`。

2. 如果提供了协议类型 `ARGS...`，则自动把这些协议的高特权值设置为 `ahightlevel`。

因此，当前约定是：

- `ready key = 协议高特权值 + 1`

举例：

- `set_ready(..., e_hightlevel_db)`：
  - ready 条件内部 key 为 `2`
  - 用于推进该条件的协议高特权值为 `1`

- `set_ready<np_actorserver_connect>(..., e_hightlevel_custom)`：
  - ready 条件内部 key 为 `11`
  - `np_actorserver_connect` 的特权值为 `10`

## 3. `nready` 的判定方式

定义位置：`public/cpp/actor/actor_base/core/actor_base.cpp`

### 3.1 `is_ready()`

```cpp
bool nready::is_ready()
{
	for (auto& [lkey, lvec] : m_ready)
	{
		for (auto& litem : lvec)
		{
			if (!litem.m_fun())
			{
				return false;
			}
		}
	}
	return true;
}
```

含义：

- 只有所有 ready 条件都满足，actor 才算完全 ready。
- 一旦某个条件不满足，普通消息不再继续执行。

### 3.2 `hightlevel_ready()`

```cpp
int32_t nready::hightlevel_ready()
{
	for (auto& [lkey, lvec] : m_ready)
	{
		for (auto& litem : lvec)
		{
			if (!litem.m_fun())
			{
				return lkey;
			}
		}
	}
	return -1;
}
```

含义：

- 返回第一个未满足 ready 条件对应的内部 key。
- 若全部 ready，则返回 `-1`。

由于 `m_ready` 是 `std::map<int32_t, ...>`，所以会按 key 从小到大检查。
这意味着当前 ready 阶段天然有先后顺序：

- key 小的阶段优先
- key 大的阶段后置

## 4. actor 队列结构

定义位置：`public/cpp/actor/actor_base/core/actor.cpp`

当前 actor 内部有两类队列：

- `m_list`：普通消息 FIFO 队列
- `m_hightlist`：特权消息队列，按 `highvalue` 分桶

定义：

```cpp
std::list<handle_pram> m_list;
std::map<int32_t, std::list<handle_pram>> m_hightlist;
```

语义：

- 普通消息只在 actor 完全 ready 后处理。
- 特权消息用于推进 ready 或处理早期控制流。
- `m_hightlist` 的 key 越小，特权越高，越早被处理。

## 5. 消息入队规则

定义位置：`public/cpp/actor/actor_base/core/actor.cpp`

### 5.1 普通消息

```cpp
if (lhigh <= 0)
{
	m_list.emplace_back(apram);
	return ready().is_ready();
}
```

含义：

- 高特权值 `<= 0` 的消息视为普通消息。
- 进入 `m_list`。
- 返回值表示：当前 actor 是否已经 ready。

这个返回值会被 `actor_manage` 用来决定：

- 当前 actor 如果是 `free`，是否需要立刻进入调度队列。

### 5.2 特权消息

```cpp
const int32_t loldhigh = hight_value();
m_hightlist[lhigh].emplace_back(apram);
return loldhigh == -1 || lhigh < loldhigh;
```

含义：

- `lhigh > 0` 的消息进入特权队列。
- 如果这是第一条特权消息，或者这次来了更高优先级的消息，则返回 `true`。

这个返回值的目的：

- 避免 actor 在 `free` 状态下收到了特权消息却没有被重新调度。

该逻辑解决了一个重要问题：

- 若特权消息已入队，但 actor 不被唤醒，则 ready 永远无法推进。

## 6. actor 执行规则

定义位置：`public/cpp/actor/actor_base/core/actor.cpp`

### 6.1 先执行特权消息

`actor_handle()` 先取出 `m_hightlist`，再判断当前 ready 阶段：

```cpp
int32_t lvalue = ready().hightlevel_ready();
```

然后按以下规则处理：

```cpp
if (lvalue == -1 || lvalue > itor->first)
```

当前语义可解释为：

- 如果全部 ready，所有特权消息都可执行。
- 如果尚未全部 ready，则只执行“特权值小于当前未满足阶段 key”的消息。

结合当前 `set_ready()` 的设计：

- DB ready 条件存在 `key = 2`
- DB 激活协议特权值是 `1`
- `2 > 1`，允许执行

- custom ready 条件存在 `key = 11`
- custom 激活协议特权值是 `10`
- `11 > 10`，允许执行

### 6.2 特权消息执行完后，再决定是否处理普通消息

```cpp
if (!ready().is_ready())
{
	return false;
}
```

只有当所有 ready 条件满足时，才开始处理 `m_list` 中的普通消息。

这保证了：

- 普通业务逻辑不会在 DB/NSP/自定义前置条件未满足时过早执行。

## 7. 当前框架下的 ready 阶段示例

### 7.1 DB ready

定义位置：`public/cpp/actor/actor_base/core/actor_base.cpp`

```cpp
ready().set_ready("db load", [this]()
	{
		return m_dbclient == nullptr || m_dbclient->isloadfinish();
	}, e_hightlevel_db
);
```

含义：

- DB 类型 actor 注册一个名为 `"db load"` 的 ready 条件。
- 内部 ready key 为 `2`。
- 该条件只有在全部 dbclient load 完成后才满足。

### 7.2 custom ready

定义位置：`public/cpp/actor/actor_logic/actor_login/actor_login.cpp`

```cpp
ready().set_ready<np_actorserver_connect>("game or gatewa not connect", [this]()
	{
		return !(m_game.empty() || m_gateway.empty());
	}, e_hightlevel_custom
);
```

含义：

- `actor_login` 只有在 game 与 gateway 至少建立连接后才算完全 ready。
- 该 ready 条件内部 key 为 `11`。
- `np_actorserver_connect` 会被自动赋予 `10` 的特权值，用于推进 custom ready。

## 8. 协议注册约定

### 8.1 DB 协议

定义位置：`public/cpp/actor/protocol/nactor_auto.cpp`

当前 DB 协议注册为：

```cpp
tprotocol::tp_customs<
	np_actordb_load<DBTYPE, TDB>
	, np_actordb_load_response<DBTYPE, TDB>
	, np_actordb_save<DBTYPE, TDB>
	, np_actordb_delete<DBTYPE, TDB>
	, np_actortime_db_cache<TDB>
>(-1, e_hightlevel_db);
```

当前含义：

- 这组 DB 相关协议统一属于 DB 特权层。
- 可以在普通 ready 尚未完成时先执行。

注意：

- 当前实现按整组 DB 协议赋值。
- 若后续需要进一步区分“仅 load response 应推进 ready，而 save/delete/cache 不应提前跑”，需要再细分。

### 8.2 NSP 协议

定义位置：`public/cpp/actor/generated/auto/register_channel_db.h`

当前 NSP 相关协议注册为：

```cpp
tprotocol::tp_customs<
	np_channel_data<T>
	, np_channel_register<T>
	, np_channel_register_reply<T>
	, np_channel_exit<T>
	, np_channel_dataid_sync<T>
>(-1, e_hightlevel_nsp);

tprotocol::tp_customs<
	np_channel_check<T>
>(-1, 0);
```

当前含义：

- 主要 NSP 数据同步、注册、退出、dataid 同步消息使用 `e_hightlevel_nsp`。
- `np_channel_check<T>` 仍为普通层 `0`。

这里的设计取舍是：

- `np_channel_check<T>` 是轮询探测消息，不提升特权，避免无意义空转。
- 真正推进 NSP 同步状态的核心消息在 `e_hightlevel_nsp` 层。

## 9. 当前实现的维护约定

### 9.1 新增一个 ready 阶段时

应同时明确三件事：

1. 该 ready 条件本身是什么。
2. 哪些协议负责推进该 ready 条件。
3. 该 ready 条件应该排在 DB、NSP、custom 哪个阶段之前或之后。

### 9.2 新增一个“推进 ready 的协议”时

必须明确它属于哪一个 `e_hightlevel`。

否则容易出现两类问题：

- 消息被当普通消息，ready 无法推进。
- 消息特权层级错误，越过不应越过的阶段。

### 9.3 若修改了代码生成结果，必须同步修改模板

当前相关模板位于：

- `server/tools/makeproto/core/xml_protocol.h`

如果只改生成结果、不改模板，则下次重新生成代码时会回退。

## 10. 常见错误

### 10.1 只改 `actor_handle()`，不改 `set_ready()` 或协议注册

`ready`、`高特权值`、`调度比较规则` 必须一起看。
单改其中一处，通常会引入新的错配。

### 10.2 把推进 ready 的协议当成普通消息

如果某类消息负责“让 ready 变为 true”，它不能仅依赖 `ready().is_ready()` 后再执行。
否则会形成自锁：

- ready 没完成
- 消息进普通队列
- 普通队列不消费
- ready 永远完不成

### 10.3 把不同阶段的协议错误注册到同一层

例如：

- 把 NSP 协议注册到 DB 层
- 或把 custom ready 激活协议注册成普通消息

这会导致 ready 推进顺序紊乱。

## 11. 当前版本的一句话总结

当前实现采用如下模型：

- `e_hightlevel` 表示“推进某阶段 ready 的特权等级”
- `ready` 内部 key 存为 `e_hightlevel + 1`
- actor 在 ready 未完成时，只执行“特权值小于当前未满足 ready key”的消息
- 普通消息始终要等 `ready().is_ready()` 为真后才会执行

因此，当前框架中的 `ready` 与 `hight` 不是两个独立机制，而是一个整体：

- `ready` 决定普通消息能否执行
- `hight` 决定哪些消息可以在 ready 尚未完成时先执行
- 两者共同完成 actor 从“初始化阶段”到“完全可用阶段”的过渡
