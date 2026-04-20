# public/cpp 公共基础库

`public/cpp` 是仓库中复用度最高的 C++ 源码区域。运行时代码、测试和本地工具均直接依赖它。

## 目录职责

- `actor`
  Actor 框架、运行时契约、生成胶水代码和业务逻辑。
- `net`
  传输层与会话层，包括 TCP、UDP、WebSocket 和 KCP 支持。
- `tools`
  共享基础设施，如日志、配置、序列化、数据库辅助、定时器和命令行支持。
- `third_party`
  仓库内嵌的第三方源码与头文件。

## Actor 目录布局

`public/cpp/actor` 按职责分为以下层次：

- `actor_base`
  Actor 基础设施。
  - `core`：Actor 基础类型与生命周期原语
  - `db`：DB Actor 适配器与 DB 客户端辅助
  - `dispatch`：消息信封与分发表
  - `id`：`nguid` 与路由辅助
  - `nsp`：NSP 读写与服务端辅助
  - `runtime`：调度器与工作线程支持
  - `script`：脚本运行时集成
  - `timer`：Actor 定时器支持

- `actor_logic`
  业务与领域 Actor。
  - `access`：登录、网关、CSV 和节点相关 Actor
  - `example`：示例与教程 Actor
  - `forward`：C2G 和 G2C 转发注册
  - `misc`：新生成的未分类 Actor 暂存区
  - `ops`：管理、日志、机器人和测试相关 Actor
  - `player`：角色、创建、简报和键值 Actor
  - `social`：家族、好友、聊天、邮件、公告和排行榜 Actor
  - `world`：世界事件与活动管理

- `generated`
  仅包含生成代码。
  - `auto`：Actor 胶水代码、协议绑定、脚本绑定
  - `pb`：Protobuf 输出

- `protocol`
  共享 Actor 协议与路由契约。

- `tab`
  表/配置映射与运行时查找辅助。

## 放置规则

当代码与 Actor 生命周期、路由、协议契约或 Actor 持有的领域数据紧密耦合时，应放在 `actor` 下。

纯通用的字符串/容器工具、独立的网络封装或仅用于构建的辅助逻辑不应放在 `actor` 下。

## 维护说明

- 新增 `actor_base` 代码应放入正确的子层，而非回退到扁平根布局。
- 新增 `actor_logic` 模块应先选择领域分类再添加文件。
- 生成代码应保留在 `actor/generated` 下，不应与手写逻辑混合。
