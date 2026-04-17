/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements client-type route actor construction and cross-node forwarding.

#include "actor/actor_logic/actor_client/actor_client.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/nregister.h"
#include "actor/actor_base/core/naddress.h"
#include "net/server_session.h"
#include "net/tcp/ntcp.h"

#include <format>

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
					.m_id	= nconfig.nodeid()
				},
				.m_weight	= 0x7fffffff,
				.m_normalwarn = 0x7fffffff,
				.m_highwarn = 0x7fffffff,
			})
	{
	}

	i64_actorid actor_client::actorid()
	{
		return nguid::make(ACTOR_CLIENT, tab_self_area, nconfig.nodeid());
	}

	void actor_client::nregister()
	{
		// Set fallback handler for unregistered protocols (forward to target node)
		nrfun<actor_client>::instance().set_notfindfun(
			[](int, handle_pram& apram) 
			{
				naddress::forward(apram);
			}
		);

		// Register protocol handlers
		register_handle<actor_client
			, np_actornode_register_response
			, np_actorclient_node_connect
			, np_actornode_connect_task
			, np_actor_gatewayid_updata
			, np_actornode_update_mass
			, np_actor_server_register
			, np_connect_actor_server
			, np_actornode_update
		>();
	}

	void set_node(int32_t aserverid, int asession)
	{
		const tab_servers* tab = ttab_servers::instance().tab(nnodeid::tid(aserverid));
		if (tab == nullptr)
		{
			return;
		}
		nactornode lnode;
		lnode.m_name		= tab->m_name;
		lnode.m_serverid	= aserverid;
		lnode.m_nodetype	= tab->m_type;
		naddress::set_node(lnode);
		naddress::set_session(aserverid, asession);
	}

	bool actor_client::handle([[maybe_unused]] const message<np_connect_actor_server>& adata)
	{
		const auto* lparm = adata.get_data();
		const i32_serverid lserverid = lparm->m_serverid;
		const tab_servers* ltab = ttab_servers::instance().const_tab();
		if (ltab == nullptr || ttab_servers::instance().tab(nnodeid::tid(lserverid)) == nullptr)
		{
			tools::no_core_dump();
			return true;
		}

		i64_actorid	lactorid = id_guid();
		i64_actorid lactorserve = actor_server::actorid();
		set_node(lserverid, lparm->m_session);
		naddress::actor_address_add(lserverid, lactorserve);

		// Registernode
		np_actornode_register lpram
		{
			.m_node
			{
				.m_name = std::format(
					"node<id:{},type:{},name:{},tcount:{},area:{}>"
					, ltab->m_id, tools::em<NODE_TYPE>::name(ltab->m_type), ltab->m_name, ltab->m_tcount, ltab->m_area
				),
				.m_nodetype = ltab->m_type,
				.m_serverid = nconfig.nodeid(),
			}
		};
		actor_manage::instance().get_type(lpram.m_node.m_actortype);

		naddress::ergodic([&lpram](const naddress::map_guidserver& aactorserver, const naddress::map_servernode&, const naddress::map_rolegateway&)
			{
				lpram.m_add.reserve(aactorserver.size());
				for (const auto& [lguid, lserverid] : aactorserver)
				{
					if (lpram.m_node.m_serverid == lserverid)
					{
						lpram.m_add.emplace_back(lguid);
					}
				}
				return true;
			}
		);
		ntcp::instance().send(lparm->m_session, lpram, lactorserve, lactorid);
		return true;
	}

	void actor_client::actor_server_register(i32_serverid aserver)
	{
		if (nconfig.nodetype() == NODE_TYPE::ROBOT)
		{
			return;
		}
		ntcp::instance().connect(aserver, [this, aserver](int asession)
			{
				auto pro = std::make_shared<np_connect_actor_server>();
				pro->m_serverid = aserver;
				pro->m_session = asession;
				send_actor(id_guid(), id_guid(), pro);
			}, false, true
		);
	}

	void actor_client::actor_server_register()
	{
		send_actor(id_guid(), id_guid(), std::make_shared<np_actor_server_register>());
	}

	bool actor_client::handle([[maybe_unused]] const message<np_actor_server_register>& adata)
	{
		// Connect to ActorServer nodes and register this node
		if (auto ltype = ttab_servers::instance().nodetype(); nconfig.nodetype() != ltype || ltype == ngl::ACTORSERVER || ltype == ngl::ROBOT)
		{
			tools::no_core_dump();
			return true;
		}
		for (auto tab = ttab_servers::instance().const_tab(); int32_t id : tab->m_actorserver)
		{
				actor_server_register(nnodeid::nodeid(static_cast<int16_t>(id), 1));
		}
		return true;
	}

	bool actor_client::isactiv_connect(i32_serverid aserverid)
	{
		return nconfig.nodeid() > aserverid;
	}

	void actor_client::activ_connect(i32_serverid aserverid)
	{
		if (isactiv_connect(aserverid))
		{
			ntcp::instance().connect(aserverid, [this, aserverid](i32_session asession)
				{
					set_node(aserverid, asession);
					np_actorclient_node_connect pro;
					pro.m_id = nconfig.nodeid();
					ntcp::instance().send(asession, pro, nguid::moreactor(), id_guid());
				}, false, true
			);
		}
	}

	bool actor_client::handle([[maybe_unused]] const message<np_actornode_register_response>& adata)
	{
		if (nconfig.nodetype() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto* lparm = adata.get_data();
		if (ttab_servers::instance().const_tab() == nullptr)
		{
			tools::no_core_dump();
			return true;
		}
		for (const auto& lnode : lparm->m_vec)
		{
			if (nconfig.nodeid() != lnode.m_serverid && server_session::sessionid(lnode.m_serverid) == -1)
			{
				// Compare node IDs to decide which side initiates the connection
				activ_connect(lnode.m_serverid);
			}
		}
		return true;
	}

	void node_update(actor_client* aclient, i32_serverid alocalserverid, i32_session asession)
	{
		np_actornode_update lpro;
		lpro.m_id = alocalserverid;
		naddress::ergodic([&lpro, alocalserverid](const naddress::map_guidserver& aactorserver, const naddress::map_servernode&, const naddress::map_rolegateway&)
			{
				lpro.m_add.reserve(aactorserver.size());
				for (const auto& [lguid, lserverid] : aactorserver)
				{
					if (alocalserverid == lserverid)
					{
						lpro.m_add.emplace_back(lguid);
					}
				}
				return true;
			}
		);
		ntcp::instance().send(asession, lpro, nguid::moreactor(), aclient->id_guid());
	}

	bool actor_client::handle([[maybe_unused]] const message<np_actorclient_node_connect>& adata)
	{
		if (nconfig.nodetype() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto* lparm = adata.get_data();
		const pack* lpack = adata.get_pack();

		i32_serverid lserverid = lparm->m_id;

		if (lserverid == nconfig.nodeid())
		{
			tools::no_core_dump();
			return true;
		}

		node_update(this, nconfig.nodeid(), lpack->m_id);

		server_session::add(lserverid, lpack->m_id);

		set_node(lserverid, lpack->m_id);

		// Connection
		if (isactiv_connect(lserverid) == false)
		{
			np_actorclient_node_connect pro;
			pro.m_id = nconfig.nodeid();
			ntcp::instance().send(lpack->m_id, pro, nguid::moreactor(), id_guid());
		}

		set_connect_fnish(lserverid);
		connect_fnish();

		// Currentnodetypeif server, connection node [GAME/GATEWAY]
		NODE_TYPE lservertype = ttab_servers::instance().node_type(nnodeid::tid(lserverid));
		log_error()->print("np_actorclient_node_connect [{}:{}]", nnodeid::tid(lserverid), nnodeid::tcount(lserverid));
		if (nconfig.nodetype() == ngl::LOGIN && (lservertype == ngl::GAME || lservertype == ngl::GATEWAY))
		{
			auto pro = std::make_shared<np_actorserver_connect>();
			pro->m_serverid = lserverid;
			actor::send_actor(nguid::make_self(ACTOR_LOGIN), guid(), pro);
		}
		return true;
	}

	bool actor_client::handle([[maybe_unused]] const message<np_actornode_update>& adata)
	{
		const auto* lparm = adata.get_data();
		naddress::actor_address_add(lparm->m_id, lparm->m_add);
		naddress::actor_address_del(lparm->m_del);
		for (const auto& [lguid, lserverid] : lparm->m_rolegateway)
		{
			naddress::gatewayid_add(lguid, lserverid);
		}		
		return true;
	}
	
	bool actor_client::handle([[maybe_unused]] const message<np_actornode_update_mass>& adata)
	{
		const auto* lparm = adata.get_data();
		const int32_t lthreadid = adata.thread();

		message lmessage(lthreadid, adata.get_shared_pack(), const_cast<np_actornode_update*>(&lparm->m_mass));
		handle(lmessage);

		if (nconfig.nodetype() != NODE_TYPE::ROBOT)
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
		if (m_connectfun.empty())
		{
			return;
		}
		for (auto itor = m_connectfun.begin(); itor != m_connectfun.end(); )
		{
			if (m_connectserverid.find(itor->first) == m_connectserverid.end())
			{
				++itor;
				continue;
			}
			for (auto& fun : itor->second)
			{
				fun();
			}
			itor = m_connectfun.erase(itor);
		}
	}
	
	bool actor_client::handle([[maybe_unused]] const message<np_actornode_connect_task>& adata)
	{
		if (nconfig.nodetype() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto* lparm = adata.get_data();
		if (m_connectserverid.find(lparm->m_serverid) != m_connectserverid.end())
		{
			lparm->m_fun();
			return true;
		}
		m_connectfun.try_emplace(lparm->m_serverid).first->second.emplace_back(lparm->m_fun);
		return true;
	}
	
	bool actor_client::handle([[maybe_unused]] const message<np_actor_gatewayid_updata>& adata)
	{
		if (nconfig.nodetype() == NODE_TYPE::ROBOT)
		{
			return true;
		}
		const auto* lparm = adata.get_data();
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
