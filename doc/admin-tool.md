# 游戏运营后台工具

## 概述

`admin/` 目录是一套基于 PHP 的游戏运营后台（GM 工具），提供 Web 界面供运营人员管理游戏服务器。后台通过 TCP Socket 与游戏服务器通信，发送 JSON 格式的 GM 指令，接收并展示执行结果。

## 目录结构

```text
admin/
  ├─ index.php              主页面（功能导航面板）
  ├─ config.php             数据库与服务器连接配置
  ├─ auth.php               认证与权限校验
  ├─ login.php / logout.php 登录/登出
  ├─ actionmanager.php      权限动作定义（两级层次结构）
  ├─ socketbyte.php         TCP Socket 通信封装
  ├─ render.php             通用响应渲染器
  ├─ serverls.php           服务器选择下拉框（WORLD 节点）
  ├─ servertype.php         服务器类型多选框
  │
  ├─ account/               GM 账号管理
  │   ├─ gmuser.php         账号列表/新增/删除/重置密码
  │   ├─ chmod.php          权限分配
  │   └─ chpass.php         修改自身密码
  │
  ├─ ban/                   封号管理
  │   ├─ ban.php            封号操作
  │   └─ query_ban.php      查询封号记录
  │
  ├─ chat/                  禁言管理
  │   └─ ban_chat.php       禁言操作
  │
  ├─ db/                    数据库查询
  │   ├─ query.php          单条查询
  │   ├─ query_page.php     分页查询
  │   └─ update.php         数据修改
  │
  ├─ family/                家族管理
  │   ├─ query_family.php   查询家族
  │   └─ rename_family.php  家族改名
  │
  ├─ guid/                  GUID 查询
  │   └─ get_guid.php       获取 GUID
  │
  ├─ mail/                  邮件管理
  │   ├─ send_mail.php      发送邮件
  │   ├─ query_mail.php     查询邮件
  │   └─ del_mail.php       删除邮件
  │
  ├─ notice/                公告管理
  │   ├─ add_notice.php     添加公告
  │   ├─ query_notice.php   查询公告
  │   └─ del_notice.php     删除公告
  │
  ├─ openserver/            开服管理
  │   ├─ set_open.php       设置开服
  │   └─ query_open.php     查询开服状态
  │
  ├─ recharge/              充值管理
  │   ├─ gm_recharge.php    GM 充值
  │   └─ query_order.php    订单查询
  │
  ├─ protocol/              协议查询
  │   └─ all_protocol.php   查看全部协议
  │
  ├─ ranklist/              排行榜
  │   └─ query_rank.php     查询排行
  │
  ├─ server_stat/           服务器状态
  │   └─ stat.php           查看状态
  │
  ├─ actor/                 系统操作
  │   └─ close_actor.php    关闭 Actor
  │
  ├─ time/                  时间管理
  │   ├─ get_time.php       查询服务器时间
  │   └─ set_time.php       设置服务器时间
  │
  ├─ log/                   操作日志
  │   └─ gmlog_html.php     GM 操作日志查询
  │
  ├─ sql/                   数据库初始化
  │   └─ init.sql           建表脚本
  │
  └─ sysmail/               系统邮件（调试用）
      └─ mail.php           通过 SMTP 发送系统邮件
```

## 技术架构

### 认证与权限

- 基于 PHP Session 的登录机制
- `db_admin` 表存储 GM 账号，密码使用 MD5 哈希
- 权限系统为两级层次结构：功能组 → 具体操作
- 每个操作有唯一数字 ID，用户权限存储为 JSON 数组或 `"all"`
- `check_action($id)` 在每个操作页面入口校验权限
- 所有操作记录到 `db_gmlog` 审计日志

### 权限动作列表

| 功能组 | 组 ID | 操作 |
|--------|-------|------|
| 我的账号 | 1 | 修改密码(101)、退出登录(102)、刷新功能列表(103) |
| 账号管理 | 2 | GM 账号管理(201) |
| 封号管理 | 3 | 封号(301)、查询封号(302) |
| 禁言管理 | 4 | 禁言(401) |
| 数据库查询 | 5 | 查询(501)、分页查询(502)、修改数据(503) |
| 家族管理 | 6 | 查询家族(601)、家族改名(602) |
| GUID 查询 | 7 | 获取 GUID(701) |
| 邮件管理 | 8 | 发送邮件(801)、查询邮件(802)、删除邮件(803) |
| 公告管理 | 9 | 添加公告(901)、查询公告(902)、删除公告(903) |
| 开服管理 | 10 | 设置开服(1001)、查询开服(1002) |
| 充值管理 | 11 | GM 充值(1101)、订单查询(1102) |
| 协议查询 | 12 | 全部协议(1201) |
| 排行榜 | 13 | 查询排行(1301) |
| 服务器状态 | 14 | 查看状态(1401) |
| 系统操作 | 15 | 关闭 Actor(1501) |
| 时间管理 | 16 | 查询时间(1601)、设置时间(1602) |
| 操作日志 | 17 | 查看日志(1701) |

### 通信机制

后台通过 `socketbyte.php` 封装的 TCP Socket 与游戏服务器通信：

1. 从 `db_server` 表获取目标服务器的 IP 和端口
2. 建立 TCP 连接
3. 发送 JSON 格式的 GM 指令
4. 等待并解析响应

#### 消息格式

```json
{
  "actor_name": "ACTOR_XXX",
  "operator": "操作名",
  "data": { ... }
}
```

#### 数据包结构

- 包头：4 字节长度（大端序）
- 包体：JSON 字符串

#### 目标 Actor 映射

| 功能 | 目标 Actor | 操作符 |
|------|-----------|--------|
| 封号/解封 | ACTOR_LOGIN | ban / query_ban |
| 禁言 | ACTOR_CHAT | ban_chat |
| 数据库查询 | ACTOR_DB | query / query_page / update |
| 家族查询 | ACTOR_FAMILY | query_family / rename_family |
| GUID 查询 | ACTOR_BRIEF | get_guid |
| 发送邮件 | ACTOR_MAIL | send_mail / query_mail / del_mail |
| 公告管理 | ACTOR_NOTICE | add_notice / query_notice / del_notice |
| 开服管理 | ACTOR_MANAGE | set_open / query_open |
| GM 充值 | ACTOR_RECHARGE | gm_recharge / query_order |
| 协议查询 | ACTOR_GM | all_protocol |
| 排行榜 | ACTOR_RANKLIST | query_rank |
| 服务器状态 | ACTOR_GM | server_stat |
| 关闭 Actor | ACTOR_GM | close_actor |
| 时间管理 | ACTOR_GM | get_time / set_time |

### 数据库

后台使用独立的 MySQL 数据库 `lbgmsys`，包含以下表：

- `db_admin` — GM 账号（用户名、MD5 密码、权限 JSON）
- `db_gmlog` — 操作审计日志（操作人、动作、详情、IP、时间）
- `db_recharge` — 充值订单记录
- `db_server` — 服务器注册表（区服、IP、端口、节点类型）

### 服务器节点类型

| 类型 ID | 节点名称 |
|---------|----------|
| 1 | DB |
| 2 | ACTOR |
| 3 | GAME |
| 4 | GATEWAY |
| 5 | LOGIN |
| 6 | ROBOT |
| 7 | WORLD |
| 8 | LOG |
| 9 | RELOADCSV |

## 部署说明

1. 导入 `admin/sql/init.sql` 初始化 GM 数据库
2. 修改 `admin/config.php` 配置数据库连接信息
3. 在 `db_server` 表中注册所有游戏服务器节点
4. 部署到支持 PHP 的 Web 服务器（需要 `mysql` 扩展）
5. 默认管理员账号在 `init.sql` 中创建

## 常见改动路径

- 新增 GM 功能：在 `actionmanager.php` 注册新动作 ID → 创建对应 PHP 文件 → 编写 HTML 表单和处理逻辑
- 修改权限：更新 `actionmanager.php` 中的 `$ActionList` 数组
- 新增服务器节点：在 `db_server` 表中插入记录
- 调整通信协议：修改 `socketbyte.php` 中的包头格式

## 已知限制

- 使用已废弃的 `mysql_*` 扩展，不兼容 PHP 7+
- 密码使用 MD5 哈希，安全性不足
- GM Socket 通信无鉴权握手
- `$_POST` 参数未做充分校验
- 配置文件中硬编码数据库密码
