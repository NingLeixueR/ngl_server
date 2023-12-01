#pragma once

#include "actor.h"
#include "actor_protocol.h"
#include "xmlnode.h"
#include "impl.h"
#include "actor_address.h"
#include "net.h"

namespace ngl
{
	class actor_client : public actor
	{
		struct impl_actor_client;
		ngl::impl<impl_actor_client> m_impl_actor_client;

		actor_client();
		virtual ~actor_client();
	public:
		friend class actor_instance<actor_client>;
		static actor_client& getInstance() 
		{ 
			return actor_instance<actor_client>::instance(); 
		}
		
		// 注册需要处理的消息
		static void actor_register();
		
		enum { ACTOR_TYPE = ACTOR_ADDRESS_CLIENT };
	private:
		void actor_server_register(i32_serverid aserverid);
	public:
		//# 向actor_server注册结点
		void actor_server_register();
	private:
		// 设置连接后时间 例如actor_dbclient会注册连接后事件（加载数据）
		void set_connect_fnish(i32_serverid aserverid);
		// 连接成功后调用
		void connect_fnish();
	public:
		// 注册结点
		bool handle(message<actor_node_register_response>& adata);
		// actor客户端间相互连接
		bool handle(message<actor_client_node_connect>& adata);
		// 服务器向actor客户端同步结点信息
		bool handle(message<actor_node_update>& adata);
		// 向actor客户端同步结点信息(群发)
		bool handle(message<actor_node_update_mass>& adata);
		// 连接成功后执行任务
		bool handle(message<actor_node_actor_connect_task>& adata);
		// 同步actor_role对应的gateway
		bool handle(message<actor_gateway_id_updata>& adata);
	};

}
