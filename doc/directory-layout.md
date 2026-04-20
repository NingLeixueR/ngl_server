# 目录结构

## 目标

本文档描述仓库在目录整理后的物理布局。
当前结构反映了真实的物理归属关系，而非占位性的聚合目录。

## 顶层目录

```text
client/     客户端代码与脚本
doc/        架构与模块文档
public/     运行时、工具和测试共用的源代码
server/     服务端运行时目标、本地工具和测试
cmake/      共享构建辅助模块
```

## 公共代码

```text
public/cpp/
  actor/        Actor 框架、运行时契约、生成胶水代码、业务逻辑
  net/          传输层与连接层
  tools/        通用基础设施工具
  third_party/  仓库内嵌第三方头文件与源码
```

规则：

- 可复用代码放在 `public/cpp` 下。
- 不要将仅用于构建的辅助代码混入 `public/cpp`。
- 生成代码与手写逻辑应明确分离。

## Actor 目录布局

```text
public/cpp/actor/
  actor_base/
    core/       Actor 基础类型与通用运行时抽象
    db/         Actor 数据库适配器与 DB 客户端辅助
    dispatch/   消息信封与分发注册
    id/         nguid 与路由地址辅助
    nsp/        NSP 读写与服务端辅助
    runtime/    调度器与工作线程支持
    script/     脚本运行时集成
    timer/      Actor 定时器支持

  actor_logic/
    access/     登录、网关、会话、CSV、节点相关 Actor
    example/    示例与教程 Actor
    forward/    C2G / G2C 转发注册
    misc/       新生成的未分类 Actor 暂存区
    ops/        管理、日志、机器人、测试相关 Actor
    player/     角色、创建、简报、键值等玩家相关 Actor
    social/     家族、好友、聊天、邮件、公告、排行榜
    world/      世界事件与活动管理

  generated/
    auto/       生成的 Actor 胶水代码与脚本/协议绑定
    pb/         生成的 Protobuf 源码

  protocol/     共享 Actor 协议与路由契约
  tab/          表/配置映射与运行时查找辅助
```

## Server 目录布局

```text
server/
  runtime/
    libnglcore/
      core/
        bootstrap/  启动与节点引导代码
    node/
  tools/
    rebuild/
    makeproto/
    idl/
  tests/
```

## 迁移说明

本次目录整理完成了以下变更：

- `server/` 现在使用真实的 `runtime / tools / tests` 归属根目录。
- `server/runtime/libnglcore/core/bootstrap/` 现在包含启动组装代码。
- `public/cpp/actor/actor_base/` 已按运行时职责拆分。
- `public/cpp/actor/actor_logic/` 保留了原有的扁平 Actor 模块布局。
- 生成的 Actor 代码现在位于 `public/cpp/actor/generated/` 下。
- 工具生成器已更新，后续输出将保持扁平 Actor 模块布局。

## 遗留风险

- 本轮变更后尚未重新执行构建验证。
