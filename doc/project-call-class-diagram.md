# 项目主干调用类图

本文档聚焦本次补充中文注释时涉及的主干类，不包含以下内容：

- `third_party`、`pb`、`auto`、测试代码
- 纯数据结构或生成协议定义
- 业务层数量很多但关系重复的具体 Actor 子类

类图关系来自当前仓库源码的静态梳理，重点表达继承、组合和主要调用链路。

## 客户端主链路

```mermaid
classDiagram
direction LR

class Transport {
  <<abstract>>
  +protocol
  +connect()
  +disconnect(reason)
  +sendBinary(payload)
  +sendText(payload)
}

class TcpTransport {
  -options
  -socket
  -connectPromise
  +connect()
  +disconnect(reason)
  +sendBinary(payload)
}

class WsTransport {
  -options
  -socket
  -connectPromise
  +connect()
  +disconnect(reason)
  +sendBinary(payload)
  +sendText(payload)
}

class KcpTransport {
  -options
  -socket
  -kcp
  -isConnected
  +localPort
  +discoverPublicIp()
  +setHandshake(options)
  +connect()
  +disconnect(reason)
  +sendBinary(payload)
}

class IKcp {
  +send(payload)
  +input(payload)
  +recv()
  +update(now)
}

class NglPacketCodec {
  +xor
  +encode(options)
  +decode(frame, transport)
  +createStreamDecoder(transport)
  +peekFrameLength(source)
}

class NglStreamDecoder {
  -codec
  -transport
  -buffer
  +reset()
  +push(chunk)
}

class MessageRegistry {
  -entries
  +register(protocolId, codec, options)
  +decode(packet)
  +dispatch(client, decoded)
  +getRequiredCodec(protocolId)
}

class NglClientCore {
  +transport
  +packetCodec
  +messages
  +connect()
  +disconnect(reason)
  +sendPacket(options)
  +send(protocolId, value, header)
  +waitMessage(protocolId, options)
  +request(protocolId, value, options)
  +call(protocolId, value, expect, options)
}

class NglClient {
  +tcp(transport, options)$
  +ws(transport, options)$
  +wss(transport, options)$
  +kcp(transport, options)$
}

class BrowserDebugRecorder {
  +records
  +push(record)
  +snapshot()
}

class BrowserNglClient {
  +debugRecorder
  +connect(options)$
  +use(registration)
  +protobuf(protocolId, messageType, options)
  +raw(protocolId, options)
  +installOnWindow(name)
  +snapshotDebug()
}

class NglProjectConfig {
  +load(paths)$
  +createPacketCodec()
  +createResolver()
  +resolveLogin(area, protocol)
  +resolveServer(serverId, protocol)
  +resolveKcp(serverId, kind)
}

class NglRobotClient {
  +state
  +fromProject(options)$
  +connect()
  +disconnect(reason)
  +connectKcp(kind)
  +sendTo(channel, protocolId, value, header)
  +sendRole(protocolId, value, header)
}

Transport <|-- TcpTransport
Transport <|-- WsTransport
Transport <|-- KcpTransport
KcpTransport o--> IKcp : 封装
NglPacketCodec --> NglStreamDecoder : 创建
NglClientCore o--> Transport : 依赖
NglClientCore o--> NglPacketCodec : 编解码
NglClientCore o--> MessageRegistry : 协议注册
NglClientCore <|-- NglClient
NglClientCore <|-- BrowserNglClient
BrowserNglClient o--> BrowserDebugRecorder : 调试记录
NglRobotClient o--> NglClient : 管理 login/gateway/kcp 通道
NglRobotClient ..> KcpTransport : 建立 KCP 通道
NglRobotClient ..> NglProjectConfig : fromProject()
NglProjectConfig ..> NglPacketCodec : createPacketCodec()
```

## 服务端骨架链路

```mermaid
classDiagram
direction LR

class nready {
  +is_ready(aready)
  +set_ready(aready, fun)
  +set_readybycustom(fun)
}

class actor_base {
  +ready()
  +manage_dbclient()
  +init()
  +release()
  +save()
  +erase_actor()
  +send_actor(...)
  +send_client(...)
  +send_server(...)
  +set_timer(parm)
}

class actor {
  +init_rfun()
  +push(apram)
  +actor_handle(threadid)
  +broadcast()
  +handle_close(message)
}

class actor_manage {
  +instance()$
  +add_actor(actor, fun)
  +erase_actor(guid, fun)
  +push(actor, thread)
  +push_task_id(guid, parm)
  +push_task_type(type, parm)
  +broadcast_task(parm)
}

class actor_suspend {
  +actor_suspend()
  +~actor_suspend()
}

class actor_instance_T {
  +instance()$
}

class serviceio_info {
  +get_ioservice(threadid)
  +get_ioservice_work(threadid)
  +shutdown()
}

class service_io {
  +m_sessionid
  +m_list
  +buff()
}

class service_tcp {
  +m_socket
}

class ws_send_node {
  +is_pack()
  +is_voidpack()
  +is_text()
  +get()
}

class service_ws {
  +m_closing
  +m_ws_send_list
  +socket()
  +using_tls()
  +read_buffer()
  +visit_stream(fun)
}

class server_session {
  +add(serverId, sessionId)$
  +remove(sessionId)$
  +sessionid(serverId)$
  +serverid(sessionId)$
  +serverinfo(serverId, name)$
}

nready <-- actor_base : 组合
actor_base <|-- actor
actor_manage o--> actor : 调度执行
actor_instance_T ..> actor_manage : 单例注册
actor ..> actor_manage : 投递消息/广播
actor_base ..> actor_manage : send_actor()/erase_actor()
service_io <|-- service_tcp
service_io <|-- service_ws
service_ws o--> ws_send_node : 发送队列
server_session ..> service_tcp : 维护 session 映射
```

## 阅读建议

- 先看客户端图：`NglClientCore` 是统一入口，`Transport` 是可替换底座，`MessageRegistry` 管协议分发。
- 再看服务端图：`actor_manage` 负责调度，`actor` 负责队列执行，`actor_base` 提供发送、定时器、脚本和 DB 通用能力。
- 如需继续细化业务链路，建议下一步针对 `actor_gateway`、`actor_login`、`actor_role` 单独再拆子图。
