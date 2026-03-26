# makeproto 模块

`server/tools/makeproto` 是面向协议和数据库生成链路的本地工具模块。

## 关键目录

- `server/tools/makeproto/core`

## 职责

- 生成协议相关辅助产物。
- 生成数据库配套文件。

## 定位

- 这是本地开发工具，不属于运行时主链路。
- 应与 `server/runtime` 明确隔离。
