## 如何构造一个actor对象
构造actor对象<br/>
以actor_role为例<br/>
首先要定义actor对象的类型<br/>
在ENUM_ACTOR中插入对actor_role的类型定义ACTOR_ROLE<br/>
```
enum ENUM_ACTOR
{
	ACTOR_NONE = -1,
	//#######################  非单例  #####################
	ACTOR_ROLE = 1,
}
```
在构造函数中通过actorparm指定actor类型(type)、区服(area)、数据id(id)<br/>
是否需要加载数据(manage_dbclient) <br/>
权重(weight)用来决定单次处理消息的数量<br/>
是否支持处理广播(broadcast)<br/>
```
actor_role::actor_role(i16_area aarea, i32_actordataid aroleid, void* adata) :
	actor(
		actorparm
		{
			.m_parm
			{
				.m_type = ACTOR_ROLE,
				.m_area = aarea,
				.m_id = aroleid,
				.m_manage_dbclient = true
			},
			.m_weight = 0x7fffffff,
			.m_broadcast = true,
		})
	, m_gatewayid(((actor_switch_process_role*)(adata))->m_gatewayid)
	, m_playactorid(0)
{
	assert(aarea == ttab_servers::tab()->m_area);
}
```
在这里我们指定<br/>
m_type也就是类型为ACTOR_ROLE<br/>
m_area也就是区服为构造函数的传参aarea<br/>
m_id也就是数据id为构造函数的传参aroleid<br/>
m_manage_dbclient需要加载数据<br/>
m_weight权重为最大32位int值<br/>
m_broadcast支持广播消息的处理<br/>
同时因为我们支持了数据加载所以要实现虚函数init<br/>
```
virtual void init()
{
	m_info.set(this);
	m_bag.set(this);
	m_task.set(this);
}
```
在其中我们让dbclient与我们的actor_role对象进行关联<br/>
这样其中的数据就会自动从数据库中加载到dbclient中<br/>
下面我们还需要实现一个注册函数，用来注册我们需要处理的消息<br/>
```
static void nregister()
{
	// 定时器
	register_timer<actor_role>(&actor_role::timer_handle);
        // 自定义消息处理
	register_actor<EPROTOCOL_TYPE_CUSTOM, actor_role>(
		true
		, dregister_fun_handle(actor_role, actor_disconnect_close)
	);
        // protobuf消息处理
	register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_role>(
		true
		, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_ROLE_SYNC)
		, dregister_fun_handle(actor_role, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE)
		, dregister_fun_handle(actor_role, mforward<GM::PROBUFF_GM_RECHARGE>)
	);

	// 通过网关进行转发的客户端协议注册
	nforward::c2g();
}
```
首先我们注册了对定时器的处理，<br/>
这样actor_role中的定时任务就有处理的地方了<br/>
需要实现<br/>
```
bool actor_role::timer_handle(message<timerparm>& adata)
```
同时我们也注册了<br/>
1.自定义消息的处理<br/>
2.protobuf消息处理<br/>
3.通过网关进行转发的客户端协议注册<br/>
4.我们都需要实现其对应的消息处理<br/>
```
// CMD 协议
bool handle(message<pbnet::PROBUFF_NET_CMD>& adata);
bool handle(message<pbnet::PROBUFF_NET_ROLE_SYNC>& adata);
bool handle(message<pbnet::PROBUFF_NET_GET_TIME>& adata);
bool handle(message<pbnet::PROBUFF_NET_SWITCH_LINE>& adata);
bool handle(message<actor_send_item>& adata);
bool handle(message<actor_disconnect_close>& adata);
//玩法创建成功  记录玩法actorid
bool handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata);
bool handle(message<pbnet::PROBUFF_NET_RECHARGE>& adata);
bool handle(message<mforward<GM::PROBUFF_GM_RECHARGE>>& adata);
```
然后对actor_role与类型进行关联<br/>
在void init_nactor_type()函数中进行关联<br/>
```
dinit_atype(actor_role, ACTOR_ROLE);
```
同时自定义消息也需要定义自动生成协议号<br/>
```
在void init_protobuf::init_customs()中加入你要处理的协议
init_customs(EPROTOCOL_TYPE_CUSTOM
	, null<timerparm>
);
加入了对timerparm消息的支持
```
