## 如何给actor对象添加消息
### actor ngl支持的消息类型有两种
1. 自定义消息类型
2. protobuf消息
### 首先是对“自定义消息类型”的介绍
1. 在nprotocol.h文件中我们对自定义消息进行定义
```
	struct nactornode
	{
		std::string		m_name;// 服务器名称
		i32_serverid	m_serverid;// 服务器id
		std::vector<i16_actortype>	m_actortype;// ENUM_ACTOR_TYPE
		std::string		m_ip;// ip
		i16_port		m_port;// 端口
		def_portocol_function(nactornode, m_name, m_serverid, m_actortype, m_ip, m_port)
	};

	// ---- [actor client -> actor server] register
	// 注册结点
	struct np_actornode_register
	{
		nactornode m_node;
		std::vector<i64_actorid> m_add;
		def_portocol(np_actornode_register, m_node, m_add)
	};
```
其中np_actornode_register结构为注册结点的消息,nactornode是描述结点的结构<br/>
其中有两个非常重要的宏，其主要作用是绑定结构成员，对网络序列化进行支持<br/>
def_portocol_function用来绑定非消息结构,也就是消息结构内包含的结构<br/>
def_portocol用来绑定消息结构<br/>
2. 需要让自定义消息自动生成并绑定上协议号
在函数[void init_protobuf::init_customs()]中添加
```
	init_customs(EPROTOCOL_TYPE_CUSTOM
		, null<timerparm>
		, null<np_robot_pram>
		, null<np_actornode_register>
	);
```
至此就是如何在actor对象中定义对此消息的支持了,<br/>
因为protobuf消息与自定义消息的步骤相同，<br/>
### 我们先介绍protobuf在来看这个问题
1. 在net.proto文件中定义需要支持的消息
2. 然后自动借助工具[makeproto.exe]自动生成net_protocol.xml与netserver_protocol.xml,
其主要是协议名称与协议号的自动生成<br/>

### 在actor中对消息的支持
在actor对象中定义和实现bool handle(message<XXX>& adata);<br/>
例如
```
bool handle(message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);
bool handle(message<np_actor_senditem>& adata);
```
在actor对象的函数[static void nregister();]中添加对函数的注册
```
register_actor<EPROTOCOL_TYPE_CUSTOM, actor_role>(
	true			// 这里指的是对此消息的处理需要在actor对象关联的数据全部加载后才开始
	, dregister_fun_handle(actor_role, np_actor_senditem)
);
nforward::c2g();其中是对client消息也就是protobuf消息的处理进行注册,
因为gateway也需要注册所以对其进行了封装
```
