#pragma once

#include "actor.h"
#include "actor_protocol.h"
#include "naddress.h"
#include "xmlnode.h"
#include "impl.h"
#include "net.h"

namespace ngl
{
	// ### 每个进程都需要有一个actor_client或actor_server
	// ### 通常一个区服只有一个进程中包含actor_server 其他所有进程都必须包含actor_client
	// ### actor_client 用来纪录保存nguid与服务器id的对应关系，
	// ### actor_client相当于actor框架的路由器，为actor提供路由功能
	// ### 而actor_server负责分发各个actor_client中本地的nguid对应关系
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
		
		// ## 注册需要处理的消息
		static void nregister();
	private:
		void actor_server_register(i32_serverid aserverid);
	public:
		// ## 向actor_server注册结点
		void actor_server_register();
	private:
		// ## 设置连接后时间 例如actor_dbclient会注册连接后事件（加载数据）
		void set_connect_fnish(i32_serverid aserverid);
		// ## 连接成功后调用
		void connect_fnish();
	public:
		// ## 注册结点
		bool handle(message<actor_node_register_response>& adata);
		// ## actor客户端间相互连接
		bool handle(message<actor_client_node_connect>& adata);
		// ## 服务器向actor客户端同步结点信息
		bool handle(message<actor_node_update>& adata);
		// ## 向actor客户端同步结点信息(群发)
		bool handle(message<actor_node_update_mass>& adata);
		// ## 连接成功后执行任务
		bool handle(message<actor_node_actor_connect_task>& adata);
		// ## 同步actor_role对应的gateway
		bool handle(message<actor_gateway_id_updata>& adata);
	};
}
