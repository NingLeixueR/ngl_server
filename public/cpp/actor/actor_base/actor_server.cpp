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
					.m_area = ttab_servers::tab()->m_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	actor_server::~actor_server()
	{}

	void actor_server::nregister()
	{
		register_handle_custom<actor_server>::func<
			np_actornode_register
			, np_actornode_update
			, np_actor_gatewayid_updata
			, np_actornode_update_mass
		>(true);
	}

	bool actor_server::handle(const message<np_actornode_register>& adata)
	{
		Try
		{
			auto lrecv = adata.get_data();
			auto lpack = adata.m_pack;
			Assert(lpack != nullptr)
			Assert(naddress::set_node(lrecv->m_node))
			naddress::set_session(lrecv->m_node.m_serverid, lpack->m_id);
			naddress::actor_add(lrecv->m_node.m_serverid, lrecv->m_add);

			i32_serverid lserverid = lrecv->m_node.m_serverid;
			std::vector<i32_sessionid> lvec;
			naddress::foreach(
				[lrecv, &lvec, lpack](const actor_node_session& anode)->bool
				{
					if (lpack->m_id != anode.m_session)
						lvec.push_back(anode.m_session);
					return true;
				});
			if (!lvec.empty())
			{
				np_actornode_register_response lpram;
				lpram.m_vec.push_back(lrecv->m_node);
				nets::sendmore(lvec, lpram, nguid::moreactor(), id_guid());
			}
			{// -- 回复
				np_actornode_register_response lpram;
				naddress::foreach(
					[&adata, &lpram, lpack](const actor_node_session& anode)->bool
					{
						if (lpack->m_id != anode.m_session)
							lpram.m_vec.push_back(anode.m_node);
						return true;
					});
				nets::sendbysession(lpack->m_id, lpram, nguid::moreactor(), id_guid());
			}
			if (!lvec.empty())
			{// -- actor_client_node_update 给其他结点
				np_actornode_update lpram
				{
					.m_id = lserverid,
					.m_add = lrecv->m_add,
				};
				nets::sendmore(lvec, lpram, nguid::moreactor(), id_guid());
			}
			{
				std::map<i32_serverid, np_actornode_update> lmapprotocol;
				naddress::ergodic(
					[&lrecv, &lmapprotocol](
						const std::map<nguid, i32_serverid>& amap, 
						const std::map<i32_serverid, actor_node_session>& asession
						)->bool
					{
						std::ranges::for_each(amap, [&lrecv, &lmapprotocol, &asession](const auto& apair)
							{
								const nguid& lguid = apair.first;
								i32_serverid lserverid = apair.second;
								if (asession.contains(lserverid) == false)
									return;
								if (lrecv->m_node.m_serverid == lserverid)
									return;
								np_actornode_update& pro = lmapprotocol[lserverid];
								pro.m_id = lserverid;
								pro.m_add.push_back(lguid.id());
							});
						return true;
					});
				std::ranges::for_each(lmapprotocol, [this, &lpack](auto& item)
					{
						nets::sendbysession(lpack->m_id, item.second, nguid::moreactor(), id_guid());
					});
			}
		}Catch
		return true;
	}

	bool actor_server::handle(const message<np_actornode_update>& adata)
	{
		Try
		{
			auto lrecv = adata.get_data();
			auto lpack = adata.m_pack;
			Assert(lpack != nullptr)
			const i32_serverid lserverid = lpack->m_id;
			naddress::actor_add(lserverid, lrecv->m_add);
			naddress::actor_del(lrecv->m_del);
			if (lrecv->m_actorservermass)
			{
				// # 分发给其他结点
				std::vector<i32_sessionid> lvec;
				naddress::foreach([lserverid, &lvec](const actor_node_session& anode)->bool
					{
						if (anode.m_node.m_serverid != lserverid)
							lvec.push_back(anode.m_session);
						return true;
					}
				);
				if (!lvec.empty())
				{
					nets::sendmore(lvec, *lrecv, nguid::moreactor(), id_guid());
				}
			}
		}Catch
		return true;
	}

	bool actor_server::handle(const message<np_actor_gatewayid_updata>& adata)
	{
		auto lrecv = adata.get_data();
		auto lpack = adata.m_pack;
		if (lrecv->m_isremove)
		{
			naddress::remove_gatewayid(lrecv->m_actorid);
		}
		else
		{
			naddress::add_gatewayid(lrecv->m_actorid, lrecv->m_gatewayid);
		}
		std::vector<i32_sessionid> lvec;
		naddress::foreach(
			[&lvec, lpack](const actor_node_session& anode)
			{
				if (lpack->m_id != anode.m_session)
					lvec.push_back(anode.m_session);
				return true;
			}
		);
		if (lvec.empty() == false)
		{
			nets::sendmore(lvec, *lrecv, nguid::moreactor(), id_guid());
		}
		return true;
	}

	bool actor_server::handle(const message<np_actornode_update_mass>& adata)
	{
		auto lparm = adata.get_data();
		auto lpack = adata.m_pack;
		int32_t lthreadid = adata.m_thread;

		message<np_actornode_update> lmessage(lthreadid, lpack, (np_actornode_update*)&lparm->m_mass);

		handle(lmessage);

		if (lparm->m_fun != nullptr)
			lparm->m_fun();
		return true;
	}
}//namespace ngl