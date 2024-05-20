#pragma once

#include "nprotocol.h"
#include "naddress.h"
#include "xmlnode.h"
#include "actor.h"
#include "impl.h"

namespace ngl
{
	//#####################################################
	//# 每个进程都需要有一个actor_client或actor_server
	//# 通常一个区服只有一个进程中包含actor_server 其他所有进程都必须包含actor_client
	//# actor_client 用来纪录保存nguid与服务器id的对应关系，
	//# actor_client相当于actor框架的路由器，为actor提供路由功能
	//# 而actor_server负责分发各个actor_client中本地的nguid对应关系
	//#########################################################
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
		
		//# 注册需要处理的消息
		static void nregister();
	private:
		//# 主动向actor_server注册actor_client
		void actor_server_register(i32_serverid aserverid);

		//# 设置连接后时间 例如actor_dbclient会注册连接后事件（加载数据）
		void set_connect_fnish(i32_serverid aserverid);

		//# 连接成功后调用
		void connect_fnish();

		//# 主动连接
		void activ_connect(i32_serverid aserverid);
	public:
		//# 向actor_server注册结点
		void actor_server_register();

		//# actor_client的actorid
		static i64_actorid actorid();

		//# 注册结点
		bool handle(message<np_actornode_register_response>& adata);

		//# actor客户端间相互连接
		bool handle(message<np_actorclient_node_connect>& adata);

		//# 服务器向actor客户端同步结点信息
		bool handle(message<np_actornode_update>& adata);

		//# 向actor客户端同步结点信息(群发)
		bool handle(message<np_actornode_update_mass>& adata);

		//# 连接成功后执行任务
		bool handle(message<np_actornode_connect_task>& adata);

		//# 同步actor_role对应的gateway
		bool handle(message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl
