﻿#include "actor_server.h"
#include "nregister.h"

namespace ngl
{
	actor_server::actor_server() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_SERVER,
					.m_area = tab_self_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_server::nregister()
	{
		//# 设置未找到协议处理函数
		nrfun<actor_server, EPROTOCOL_TYPE_CUSTOM>::instance().set_notfindfun(
			[](int, handle_pram& apram)
			{
				naddress::forward(apram);
			});
		nrfun<actor_server, EPROTOCOL_TYPE_PROTOCOLBUFF>::instance().set_notfindfun(
			[](int, handle_pram& apram)
			{
				naddress::forward(apram);
			});

		register_handle_custom<actor_server>::func<
			np_actornode_register
			, np_actornode_update_server
			, np_actornode_update_mass
			, np_actor_gatewayid_updata
		>(true);
	}

	void actor_server::forward_np_actornode_register(
		const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd
	)
	{
		std::vector<i32_sessionid> lsessionvec;
		naddress::foreach([&lsessionvec, apack](const actor_node_session& asnode)
			{
				if (apack->m_id != asnode.m_session)
				{
					lsessionvec.push_back(asnode.m_session);
				}
				return true;
			});
		if (!lsessionvec.empty())
		{
			{
				np_actornode_register_response lpram;
				lpram.m_vec.push_back(anode);
				nets::sendmore(lsessionvec, lpram, nguid::moreactor(), id_guid());
			}
			{
				// -- actor_client_node_update 给其他结点
				np_actornode_update lpram
				{
					.m_id = aserverid,
					.m_add = aadd,
				};
				nets::sendmore(lsessionvec, lpram, nguid::moreactor(), id_guid());
			}			
		}
	}

	void actor_server::reply_np_actornode_register(const pack* apack, i32_serverid aserverid)
	{
		np_actornode_register_response lpram;
		naddress::foreach([&lpram, apack](const actor_node_session& anode)
			{
				if (apack->m_id != anode.m_session)
				{
					lpram.m_vec.push_back(anode.m_node);
				}
				return true;
			});
		nets::sendbysession(apack->m_id, lpram, nguid::moreactor(), id_guid());

		std::map<i32_serverid, np_actornode_update> lmapprotocol;
		naddress::ergodic([aserverid, &lmapprotocol](const std::map<nguid, i32_serverid>& amap, const std::map<i32_serverid, actor_node_session>&)
			{
				for (const std::pair<const nguid, i32_serverid>& ipair : amap)
				{
					const nguid& lguid = ipair.first;
					i32_serverid lserverid = ipair.second;
					if (aserverid == lserverid)
					{
						continue;
					}
					np_actornode_update& pro = lmapprotocol[lserverid];
					pro.m_id = lserverid;
					pro.m_add.push_back(lguid.id());
				}
				return true;
			});
		for (const auto& [_, pro]: lmapprotocol)
		{
			nets::sendbysession(apack->m_id, pro, nguid::moreactor(), id_guid());
		}
	}

	bool actor_server::handle(const message<np_actornode_register>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();

		if (!naddress::set_node(lrecv->m_node))
		{
			//tools::no_core_dump();
			return true;
		}

		i32_serverid lserverid = lrecv->m_node.m_serverid;
		naddress::set_session(lserverid, lpack->m_id);
		naddress::add_actor_address(lserverid, lrecv->m_add);

		server_session::add(lserverid, lpack->m_id);

		forward_np_actornode_register(lpack, lrecv->m_node, lserverid, lrecv->m_add);
		reply_np_actornode_register(lpack, lserverid);
		return true;
	}

	bool actor_server::handle(const message<np_actornode_update_server>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();
		const i32_serverid lserverid = lpack == nullptr?nconfig::m_nodeid:lpack->m_id;
		naddress::add_actor_address(lserverid, lrecv->m_data.m_add);
		naddress::del_actor_address(lrecv->m_data.m_del);
		// # 分发给其他结点
		std::vector<i32_sessionid> lvec;
		naddress::foreach([lserverid, &lvec](const actor_node_session& anode)->bool
			{
				if (anode.m_node.m_serverid != lserverid)
				{
					lvec.push_back(anode.m_session);
				}
				return true;
			}
		);
		if (!lvec.empty())
		{
			nets::sendmore(lvec, lrecv->m_data, nguid::moreactor(), id_guid());
		}
		return true;
	}

	void actor_server::print(const std::vector<i64_actorid>& avec)const
	{
		for (i64_actorid aactorid : avec)
		{
			log_error()->print("np_actornode_update_mass guid:{}", nguid(aactorid));
		}
	}

	bool actor_server::handle(const message<np_actornode_update_mass>& adata)
	{
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();

		auto pro = std::make_shared<np_actornode_update_server>();
		pro->m_data = lparm->m_mass;

		print(lparm->m_mass.m_add);

		handle(message(adata.thread(), lpack, pro));
		return true;
	}

	bool actor_server::handle(const message<np_actor_gatewayid_updata>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();
		if (lrecv->m_isremove)
		{
			naddress::remove_gatewayid(lrecv->m_actorid);
		}
		else
		{
			naddress::add_gatewayid(lrecv->m_actorid, lrecv->m_gatewayid);
		}
		std::vector<i32_sessionid> lsessionvec;
		naddress::foreach([&lsessionvec, lpack](const actor_node_session& anode)
			{
				if (lpack->m_id != anode.m_session)
				{
					lsessionvec.push_back(anode.m_session);
				}
				return true;
			}
		);
		if (lsessionvec.empty() == false)
		{
			nets::sendmore(lsessionvec, *lrecv, nguid::moreactor(), id_guid());
		}
		return true;
	}
}//namespace ngl