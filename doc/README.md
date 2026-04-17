# 项目文档索引

`doc` 目录用于集中说明仓库内各个主要模块的职责、边界、依赖关系和常见改动路径。

当前文档按“先总览、再模块”的方式组织。

## 阅读顺序

1. [server 模块总览](./module-server.md)
2. [public/cpp 公共基础库](./module-public-cpp.md)
3. [libnglcore 运行时核心](./module-libnglcore.md)
4. [node 可执行入口](./module-node.md)
5. [tests 测试模块](./module-tests.md)
6. [idl 代码生成工具](./module-idl.md)
7. [makeproto 协议生成工具](./module-makeproto.md)
8. [client TypeScript 客户端](./module-client.md)
9. [Actor 系统架构](./actor-system-architecture.md)
10. [消息流与协议系统](./message-flow-and-protocol.md)
11. [网络层](./network-layer.md)
12. [网关架构](./gateway-architecture.md)
13. [数据库层](./database-layer.md)
14. [配置系统](./configuration-system.md)
15. [新增 Actor 开发指南](./guide-adding-new-actor.md)
16. [Actor 就绪与高优先级调度](./actor-ready-hightlevel.md)
17. [项目主干调用类图](./project-call-class-diagram.md)

## 模块分层

```text
client
  └─ TypeScript 客户端与机器人脚本

server
  ├─ libnglcore   运行时核心库
  ├─ node         服务进程入口
  ├─ tests        GoogleTest 测试集
  ├─ idl          IDL 相关生成工具
  └─ makeproto    协议与 SQL 生成工具

public/cpp
  ├─ actor        Actor 体系与业务基础设施
  ├─ net          网络层
  ├─ tools        通用工具层
  └─ third_party  仓库内嵌第三方库
```

## 使用建议

- 想理解服务端启动流程时，先看 `module-server.md` 和 `module-libnglcore.md`。
- 想修改网络或协议封包时，先看 `module-public-cpp.md` 和 `module-client.md`。
- 想定位测试影响范围时，先看 `module-tests.md`。
- 想修改生成链路时，先看 `module-idl.md` 和 `module-makeproto.md`。

## 维护约定

- 新增模块时，优先在本目录补一页模块文档。
- 模块文档应尽量回答四个问题：
  1. 这个模块负责什么。
  2. 它依赖哪些上游模块。
  3. 哪些模块依赖它。
  4. 常见改动应该从哪里入手。
