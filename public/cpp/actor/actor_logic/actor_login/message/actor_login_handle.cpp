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

#include "actor/actor_logic/actor_login/actor_login.h"
#include "actor/actor_base/nregister.h"

namespace ngl
{
	bool actor_login::handle(const message<np_actor_disconnect_close>& adata)
	{
		auto itor = m_actorbyserver.find(adata.get_data()->m_actorid);
		if (itor == m_actorbyserver.end())
		{
			return true;
		}
		dec_freeserver_game(itor->second.m_gameserverid);
		dec_freeserver_gateway(itor->second.m_gatewayserverid);
		m_actorbyserver.erase(itor);

		printf_freeserver();
		return true;
	}
	bool actor_login::handle(const message<np_actorserver_connect>& adata)
	{
		auto lparm = adata.get_data();
		server_info ltemp
		{
			.m_id = lparm->m_serverid,
			.m_rolesize = 0
		};
		switch (ttab_servers::instance().node_type(nnodeid::tid(lparm->m_serverid)))
		{
		case ngl::NODE_TYPE::GAME:
			m_game.insert(std::make_pair(lparm->m_serverid, ltemp));
			break;
		case ngl::NODE_TYPE::GATEWAY:
			m_gateway.insert(std::make_pair(lparm->m_serverid, ltemp));
			break;
		}
		log_error()->print("message<np_actorserver_connect>:{}", lparm->m_serverid);
		return true;
	}
	bool actor_login::handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata)
	{
		auto lparm = adata.get_data();
		auto lpack = adata.get_pack();

		log_info()->print("# Login[{}][{}][{}] #",
			lparm->marea()
			, lparm->maccount()
			, lparm->mpassword()
		);
		bool iscreate = false;

		const data_modified<pbdb::db_account>* lpaccount = get_account(lparm->marea(), lparm->maccount(), lparm->mpassword(), iscreate);
		if (lpaccount == nullptr)
		{
			return true;
		}
		
		const pair_account* lppair_account = nullptr;
		MODIFIED_RETURN_CONST(lpdbaccount, *lpaccount, false);
		auto itor = m_actorbyserver.find(lpdbaccount->mid());
		if (itor == m_actorbyserver.end())
		{
			pair_account ltempaccount;
			// 生成session key
			tools::uuid_make(ltempaccount.m_session);

			std::pair<int32_t, int32_t> lpairgame;
			std::pair<int32_t, int32_t> lpairgateway;
			if (!get_freeserver_game(lpairgame) || !get_freeserver_gateway(lpairgateway))
			{
				tools::no_core_dump();
				return true;
			}

			ltempaccount.m_gameserverid = lpairgame.first;
			ltempaccount.m_gatewayserverid = lpairgateway.first;
			m_actorbyserver[lpdbaccount->mid()] = ltempaccount;
			lppair_account = &m_actorbyserver[lpdbaccount->mid()];
		}
		else
		{
			lppair_account = &itor->second;
		}

		// # 通知gateway服务器
		{
			np_actorrole_login pro
			{
				.m_session		= lppair_account->m_session,
				.m_accountid	= lpdbaccount->mid(),
				.m_account		= lparm->maccount(),
				.m_roleid		= lpdbaccount->mroleid(),
				.m_gameid		= lppair_account->m_gameserverid,
				.m_gatewayid	= lppair_account->m_gatewayserverid,
				.m_area			= (i16_area)lpdbaccount->marea(),
				.m_iscreate		= iscreate,
				.m_socketid		= adata.get_pack()->m_id,
				.m_request_actor = lpack->m_head.get_request_actor(),
			};
			ntcp::instance().send_server(pro.m_gatewayid, pro, nguid::moreactor(), id_guid());
		}	

		// # 通知client
		{
			pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE pro;
			pro.set_mroleid(lpdbaccount->mroleid());
			pro.set_marea((i16_area)lpdbaccount->marea());
			pro.set_msession(lppair_account->m_session);
			pro.set_maccount(lparm->maccount());
			pro.set_mgatewayid(lppair_account->m_gatewayserverid);
			pro.set_mgameid(lppair_account->m_gameserverid);
			ntcp::instance().send(adata.get_pack()->m_id, pro, lpack->m_head.get_request_actor(), id_guid());
		}
		
		return true;
	}
}//namespace ngl