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
#include "actor_server.h"
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

	i64_actorid actor_server::actorid()
	{
		return nguid::make(ACTOR_SERVER, tab_self_area, nguid::none_actordataid());
	}

	void actor_server::nregister()
	{
		//# 设置未找到协议处理函数
		nrfun<actor_server>::instance().set_notfindfun(
			[](int, handle_pram& apram)
			{
				naddress::forward(apram);
			}
		);

		register_handle<actor_server
			, np_actornode_register
			, np_actornode_update_server
			, np_actornode_update_mass
			, np_actor_gatewayid_updata
		>(e_ready_all);
	}

	void actor_server::forward_register(const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd)
	{
		// 同步其他结点
		std::set<i32_sessionid> lsessions;
		naddress::foreach([&lsessions, apack](const nnode_session& asnode)
			{
				if (apack->m_id != asnode.m_session)
				{
					lsessions.insert(asnode.m_session);
				}
				return true;
			}
		);
		if (!lsessions.empty())
		{
			{
				np_actornode_register_response pro;
				pro.m_vec.push_back(anode);
				ntcp::instance().send(lsessions, pro, nguid::moreactor(), id_guid());
			}
			{
				np_actornode_update pro
				{
					.m_id = aserverid,
					.m_add = aadd,
				};
				ntcp::instance().send(lsessions, pro, nguid::moreactor(), id_guid());
			}			
		}
	}

	void actor_server::reply_register(const pack* apack, i32_serverid aserverid)
	{
		np_actornode_register_response lpram;
		naddress::foreach([&lpram, apack](const nnode_session& anode)
			{
				if (apack->m_id != anode.m_session)
				{
					lpram.m_vec.push_back(anode.m_node);
				}
				return true;
			}
		);
		ntcp::instance().send(apack->m_id, lpram, nguid::moreactor(), id_guid());

		std::map<i32_serverid, np_actornode_update> lprotocols;
		naddress::ergodic(
			[aserverid, &lprotocols](const naddress::map_guidserver& amap, const naddress::map_servernode&, const naddress::map_rolegateway& arolegateway)
			{
				for (auto& [_guid, _serverid] : amap)
				{
					if (aserverid == _serverid)
					{
						continue;
					}
					np_actornode_update& pro = lprotocols[_serverid];
					pro.m_id = _serverid;
					pro.m_add.push_back(_guid.id());
					if (pro.m_rolegateway.empty())
					{
						pro.m_rolegateway = arolegateway;
					}
				}
				return true;
			}
		);
		for (const auto& [_serverid, _pro]: lprotocols)
		{
			ntcp::instance().send(apack->m_id, _pro, nguid::moreactor(), id_guid());
		}
	}

	bool actor_server::handle(const message<np_actornode_register>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();

		naddress::set_node(lrecv->m_node);

		i32_serverid lserverid = lrecv->m_node.m_serverid;
		log_error()->print("actor_server np_actornode_register [{}]", lserverid);

		naddress::set_session(lserverid, lpack->m_id);
		naddress::actor_address_add(lserverid, lrecv->m_add);

		server_session::add(lserverid, lpack->m_id);

		forward_register(lpack, lrecv->m_node, lserverid, lrecv->m_add);
		reply_register(lpack, lserverid);
		return true;
	}

	bool actor_server::handle(const message<np_actornode_update_server>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();
		
		const i32_serverid lserverid = lrecv->m_data.m_id;
		naddress::actor_address_add(lserverid, lrecv->m_data.m_add);
		naddress::actor_address_del(lrecv->m_data.m_del);
		// # 分发给其他结点
		std::set<i32_sessionid> lsession;
		naddress::foreach([lserverid, &lsession](const nnode_session& anode)->bool
			{
				if (anode.m_node.m_serverid != lserverid)
				{
					lsession.insert(anode.m_session);
				}
				return true;
			}
		);
		if (!lsession.empty())
		{
			ntcp::instance().send(lsession, lrecv->m_data, nguid::moreactor(), id_guid());
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

		if (lparm->m_fun != nullptr)
		{
			lparm->m_fun();
		}

		handle(message(adata.thread(), adata.get_shared_pack(), pro));
		return true;
	}

	bool actor_server::handle(const message<np_actor_gatewayid_updata>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.get_pack();
		if (lrecv->m_isremove)
		{
			naddress::gatewayid_del(lrecv->m_actorid);
		}
		else
		{
			naddress::gatewayid_add(lrecv->m_actorid, lrecv->m_gatewayid);
		}
		std::set<i32_sessionid> lsessionvec;
		naddress::foreach([&lsessionvec, lpack](const nnode_session& anode)
			{
				if (lpack->m_id != anode.m_session)
				{
					lsessionvec.insert(anode.m_session);
				}
				return true;
			}
		);
		if (!lsessionvec.empty())
		{
			ntcp::instance().send(lsessionvec, *lrecv, nguid::moreactor(), id_guid());
		}
		return true;
	}
}//namespace ngl