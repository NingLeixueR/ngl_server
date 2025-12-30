/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "server_session.h"
#include "actor_client.h"
#include "actor_server.h"
#include "actor_manage.h"
#include "nregister.h"
#include "naddress.h"
#include "net.h"

namespace ngl
{
	actor_client::actor_client() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_CLIENT,
					.m_area = tab_self_area,
					.m_id	= nconfig.nodeid(),
				},
				.m_weight	= 0x7fffffff
			})
	{
	}

	actor_client::~actor_client()
	{
	}

	i64_actorid actor_client::actorid()
	{
		return nguid::make(ACTOR_CLIENT, tab_self_area, nconfig.nodeid());
	}

	void actor_client::nregister()
	{
		//# 设置未找到协议处理函数
		nrfun<actor_client>::instance().set_notfindfun(
			[](int, handle_pram& apram) 
			{
				naddress::forward(apram);
			});

		//# 注册协议
		register_handle<actor_client>::func<
			np_actornode_register_response
			, np_actorclient_node_connect
			, np_actornode_connect_task
			, np_actor_gatewayid_updata
			, np_actornode_update_mass
			, np_actor_server_register
			, np_connect_actor_server
			, np_actornode_update
		>(e_ready_all);
	}

	void set_node(int32_t aserverid, int asession)
	{
		const tab_servers* ltab = ttab_servers::instance().tab(nnodeid::tid(aserverid));
		if (ltab == nullptr)
		{
			return;
		}
		nactornode lnode;
		lnode.m_name		= ltab->m_name;
		lnode.m_serverid	= aserverid;
		lnode.m_nodetype	= ltab->m_type;
		naddress::set_node(lnode);
		naddress::set_session(aserverid, asession);
	}

	bool actor_client::handle(const message<np_connect_actor_server>& adata)
	{
		int32_t lserverid = adata.get_data()->m_serverid;
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr || ttab_servers::instance().tab(nnodeid::tid(lserverid)) == nullptr)
		{
			tools::no_core_dump();
			return true;
		}

		i64_actorid	lactorid = id_guid();
		i64_actorid lactorserve = actor_server::actorid();
		set_node(lserverid, adata.get_data()->m_session);
		naddress::actor_address_add(lserverid, lactorserve);

		//注册结点
		np_actornode_register lpram
		{
			.m_node
			{
				.m_name = std::format("node<id:{},type:{},name:{},tcount:{},area:{}>", tab->m_id, em<NODE_TYPE>::name(tab->m_type), tab->m_name, tab->m_tcount, tab->m_area),
				.m_nodetype = tab->m_type,
				.m_serverid = nconfig.nodeid(),
			}
		};
		actor_manage::instance().get_type(lpram.m_node.m_actortype);

		naddress::ergodic([&lpram](const naddress::map_guidserver& aactorserver, const naddress::map_servernode&, const naddress::map_rolegateway&){
			for (const auto& item : aactorserver)
			{
				if (lpram.m_node.m_serverid == item.second)
				{
					lpram.m_add.push_back(item.first);
				}
			}
			return true;
		});
		nets::send(adata.get_data()->m_session, lpram, lactorserve, lactorid);
		return true;
	}

	void actor_client::actor_server_register(i32_serverid aactorserver)
	{
		if (nconfig.node_type() == NODE_TYPE::ROBOT)
		{
			return;
		}
		nets::connect(aactorserver, [this, aactorserver](int asession)
			{
				auto pro = std::make_shared<np_connect_actor_server>();
				pro->m_serverid = aactorserver;
				pro->m_session = asession;
				send_actor(id_guid(), id_guid(), pro);
			}, false, true
		);
	}

	void actor_client::actor_server_register()
	{
		send_actor(id_guid(), id_guid(), std::make_shared<np_actor_server_register>());
	}

	bool actor_client::handle(const message<np_actor_server_register>& adata)
	{
		// # 需要尝试连接ActorServer结点 并向其注册自己
		NODE_TYPE ltype = ttab_servers::instance().node_type();
		if (nconfig.node_type() != ltype || ltype == ngl::ACTORSERVER || ltype == ngl::ROBOT)
		{
			tools::no_core_dump();
			return true;
		}
		const tab_servers* tab = ttab_servers::instance().tab();
		for (int32_t id : tab->m_actorserver)
		{
			actor_server_register(nnodeid::nodeid(id, 1));
		}
		return true;
	}

	bool isactiv_connect(i32_serverid aserverid)
	{
		return nconfig.nodeid() > aserverid;
	}

	void actor_client::activ_connect(i32_serverid aserverid)
	{
		if (isactiv_connect(aserverid))
		{
			nets::connect(aserverid, [this, aserverid](i32_session asession)
				{
					set_node(aserverid, asession);
					np_actorclient_node_connect pro;
					pro.m_id = nconfig.nodeid();
					nets::send(asession, pro, nguid::moreactor(), id_guid());
				}, false, true);
		}
	}

	bool actor_client::handle(const message<np_actornode_register_response>& adata)
	{
		if (nconfig.node_type() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		auto lparm = adata.get_data();
		if (const tab_servers* tab = ttab_servers::instance().tab(); tab == nullptr)
		{
			tools::no_core_dump();
			return true;
		}
		for (const nactornode& node : lparm->m_vec)
		{
			if (nconfig.nodeid() != node.m_serverid && server_session::sessionid(node.m_serverid) == -1)
			{
				// # 比较id(较大的主动连接较小的)
				activ_connect(node.m_serverid);
			}
		}
		return true;
	}

	void node_update(actor_client* aclient, i32_serverid alocalserverid, i32_session asession)
	{
		np_actornode_update lpro;
		lpro.m_id = alocalserverid;
		for (const std::pair<const nguid, i32_serverid>& item : naddress::get_actorserver_map())
		{
			if (alocalserverid == item.second)
			{
				lpro.m_add.push_back(item.first);
			}
		}
		nets::send(asession, lpro, nguid::moreactor(), aclient->id_guid());
	}

	bool actor_client::handle(const message<np_actorclient_node_connect>& adata)
	{
		if (nconfig.node_type() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();

		i32_serverid lserverid = lparm->m_id;

		if (lserverid == nconfig.nodeid())
		{
			tools::no_core_dump();
			return true;
		}

		node_update(this, nconfig.nodeid(), lpack->m_id);

		server_session::add(lserverid, lpack->m_id);

		set_node(lserverid, lpack->m_id);
		naddress::set_session(lserverid, lpack->m_id);

		// 主动连接
		if (isactiv_connect(lserverid) == false)
		{
			np_actorclient_node_connect pro;
			pro.m_id = nconfig.nodeid();
			nets::send(lpack->m_id, pro, nguid::moreactor(), id_guid());
		}

		set_connect_fnish(lparm->m_id);
		connect_fnish();

		// 当前结点类型如果是登陆服务器，且连接的结点为[GAME/GATEWAY]
		NODE_TYPE lservertype = ttab_servers::instance().node_type(nnodeid::tid(lserverid));
		log_error()->print("np_actorclient_node_connect [{}:{}]", nnodeid::tid(lserverid), nnodeid::tcount(lserverid));
		if (nconfig.nodetype() == ngl::LOGIN && (lservertype == ngl::GAME || lservertype == ngl::GATEWAY))
		{
			auto pro = std::make_shared<np_actorserver_connect>();
			pro->m_serverid = lserverid;
			nguid lguid = nguid::make_self(ACTOR_LOGIN);
			handle_pram lparm = handle_pram::create(lguid, guid(), pro);
			actor_manage::instance().push_task_id(lguid, lparm);
		}
		return true;
	}

	bool actor_client::handle(const message<np_actornode_update>& adata)
	{
		auto lparm = adata.get_data();
		naddress::actor_address_add(lparm->m_id, lparm->m_add);
		naddress::actor_address_del(lparm->m_del);
		for (const auto& lpair : lparm->m_rolegateway)
		{
			naddress::gatewayid_add(lpair.first, lpair.second);
		}		
		return true;
	}
	
	bool actor_client::handle(const message<np_actornode_update_mass>& adata)
	{
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();
		int32_t lthreadid = adata.thread();

		message lmessage(lthreadid, adata.get_shared_pack(), (np_actornode_update*)&lparm->m_mass);
		handle(lmessage);

		if (nconfig.node_type() != NODE_TYPE::ROBOT)
		{
			auto pro = std::make_shared<np_actornode_update_server>();
			pro->m_data = lparm->m_mass;
			send_actor(actor_server::actorid(), id_guid(), pro);
		}
		
		if (lparm->m_fun != nullptr)
		{
			lparm->m_fun();
		}
		return true;
	}

	void actor_client::set_connect_fnish(i32_serverid aserverid)
	{
		m_connectserverid.insert(aserverid);
	}

	void actor_client::connect_fnish()
	{
		const std::set<i32_serverid>& lconnectserverid = m_connectserverid;
		if (m_connectfun.empty())
		{
			return;
		}
		for (auto itor = m_connectfun.begin(); itor != m_connectfun.end(); ++itor)
		{
			if (!lconnectserverid.contains(itor->first))
			{
				continue;
			}
			for (const auto& fun : itor->second)
			{
				fun();
			}
			itor = m_connectfun.erase(itor);
		}
	}
	
	bool actor_client::handle(const message<np_actornode_connect_task>& adata)
	{
		if (nconfig.node_type() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		auto lparm = adata.get_data();
		const std::set<i32_serverid>& lconnectserverid = m_connectserverid;
		if (lconnectserverid.contains(lparm->m_serverid))
		{
			lparm->m_fun();
			return true;
		}
		m_connectfun[lparm->m_serverid].push_back(lparm->m_fun);
		return true;
	}
	
	bool actor_client::handle(const message<np_actor_gatewayid_updata>& adata)
	{
		if (nconfig.node_type() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto lparm = adata.get_data();
		if (lparm->m_isremove)
		{
			naddress::gatewayid_del(lparm->m_actorid);
		}
		else
		{
			naddress::gatewayid_add(lparm->m_actorid, lparm->m_gatewayid);
		}
		return true;
	}
}//namespace ngl