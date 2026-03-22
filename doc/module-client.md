# client TypeScript 客户端

`client` 是仓库内独立的 TypeScript 网络层与机器人客户端模块，不是服务端的一部分，但和服务端协议严格对齐。

## 模块职责

- 提供 TypeScript 客户端封装
- 支持 `tcp`、`ws`、`wss`、`kcp`
- 提供协议封包、protobuf 编解码和消息注册能力
- 提供 smoke 与 robot 调试脚本

## 主要目录

- `src/transport`
  各类传输实现
- `src/codec`
  协议封包与 protobuf 编解码
- `src/robot`
  机器人客户端和协议辅助
- `src/browser`
  浏览器入口
- `scripts`
  smoke 与机器人测试脚本
- `debug`
  调试页面

## 运行环境

根据 `client/README.md` 和 `package.json`：

- Node.js 24+
- 浏览器仅支持 `ws` / `wss`

## 与服务端的关系

`client` 的协议头与服务端封包格式对齐：

- 固定头部掩码
- 36 字节头
- 小端长度与时间戳
- 协议号
- actor id
- request actor id

这意味着：

- 改服务端包头格式时，必须同步评估 `client`
- 改 KCP 控制命令格式时，也要同步检查 `client/src/kcp` 和 `client/src/robot`

## 常见改动入口

- 改传输层行为
  看 `src/transport`
- 改封包格式
  看 `src/codec/ngl-packet.ts`
- 改 protobuf 适配
  看 `src/codec/protobuf.ts`
- 改机器人脚本
  看 `src/robot` 和 `scripts`

## 维护建议

- 把 `client` 当作协议兼容性验证器，而不是单纯 demo。
- 每次服务端协议层改动后，优先补一条 smoke 或 robot 用例。
- 若后续前后端共享更多消息定义，可以考虑进一步稳定生成产物的导出方式。
