# 配置系统

ngl_server 的配置分为两类：XML 系统配置（运行时参数）和 CSV 数据表（游戏策划数据）。两者在启动时加载，CSV 表支持运行时热更新。

## 相关文档

- [Actor 系统架构](./actor-system-architecture.md) — 广播 tick 与挂起机制
- [新增 Actor 开发指南](./guide-adding-new-actor.md) — 如何使用配置表

## XML 系统配置：`sysconfig`

`sysconfig`（`tools/tab/xml/sysconfig.h`）提供静态访问器，在启动时通过 `sysconfig::init()` 从 XML 文件加载。

### 配置项分类

| 分类 | 配置项 | 说明 |
|------|--------|------|
| 日志 | `logwritelevel` / `logconsolelevel` | 文件/控制台日志级别 |
| 日志 | `logline` / `logflushtime` | 单文件行数 / 刷盘间隔 |
| 加密 | `xorkey` / `isxor` / `varint` | XOR 密钥 / 是否启用 / Varint 开关 |
| KCP | `kcpping` / `kcppinginterval` / `kcpsession` | Ping 间隔 / 检查间隔 / 会话标识 |
| 限速 | `rate_interval` / `rate_count` | 检测间隔（秒）/ 每秒消息数 |
| 网络 | `heart_beat_interval` / `net_timeout` | 心跳间隔 / 超时检测 |
| 会话 | `sessionwait` | 网关移除会话的等待时间（毫秒） |
| 服务器 | `open_servertime` / `head_version` | 开服时间 / 协议头版本号 |
| 节点 | `node_count(name/type)` | 按名称/类型查询节点数量 |
| 测试 | `robot_test` | 是否启用机器人测试模式 |
| 其他 | `gmurl` / `lua` / `gatewayids` | GM 地址 / Lua 目录 / 网关 ID 集合 |

日志级别枚举（`ELOGLEVEL`）：
- `ELOG_DEBUG(0)` → `ELOG_INFO(1)` → `ELOG_WARN(2)` → `ELOG_ERROR(3)`

## CSV 数据表：`csv<T>`

### 基类 `csv_base`

`csv_base`（`tools/tab/csv/ncsv.h`）定义表的通用接口：
- `load()` — 加载或重新加载 CSV 文件到内存
- `verify()` — 返回内容 MD5 哈希（用于热更新检测）
- `csvname()` — 逻辑表名
- `reload()` — 加载后的钩子函数
- `find(id)` — 无类型查找

### 模板类 `csv<T>`

`csv<T>` 继承 `csv_base`，提供类型化的表访问：
- `m_csv`：`map<int, T>` — 按 ID 索引的行数据
- `m_verify`：内容 MD5
- `tab(id)` — 按 ID 查找单行
- `tabs()` — 获取全部数据
- `foreach(func)` / `find_if(func)` — 遍历和条件查找

文件路径约定：`./{csv_base::path()}/{type_name<T>()}.csv`

### 中央注册表 `ncsv`

`ncsv` 管理所有已加载的表：
- `loadcsv<TTAB>()` — 加载并注册一张表
- `get<TTAB>()` — 获取表实例
- `tab<TAB>(id)` — 便捷的单行查找
- `foreach_verify()` — 收集所有表的 MD5（用于比对）

### 领域包装器 `ttab_*`

生成或手写的包装器（如 `ttab_chat`、`ttab_servers`、`ttab_dbload`）继承 `csv<T>` 并添加领域特定的访问方法。

## CSV 热更新：`reload_csv`

`reload_csv`（`tools/tab/csv/ncsv.h`）为每种表类型注册三个回调：
- `m_save` — 将新 CSV 内容写入磁盘
- `m_reload` — 重新读取并解析文件
- `m_readfile` — 读取原始文件内容

热更新流程：
1. GM/管理员通过 RPC 触发
2. `save()` 写入新文件
3. `reload()` 重新加载到内存
4. 使用 `actor_suspend` 挂起调度以保证一致性

注册在 `csv<T>::load()` 中自动完成（调用 `reload_csv::register_csv<T>()`）。

## 关键文件

| 文件 | 职责 |
|------|------|
| `tools/tab/xml/sysconfig.h` | XML 系统配置声明 |
| `tools/tab/xml/sysconfig.cpp` | XML 系统配置加载实现 |
| `tools/tab/csv/ncsv.h` | CSV 表注册、加载、热更新 |
| `tools/tab/csv/csv.h` | CSV 解析器 |
| `actor/tab/csvtable.h` | 生成的表结构定义 |
