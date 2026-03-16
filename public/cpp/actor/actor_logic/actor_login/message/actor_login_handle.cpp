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
// File overview: Implements message handlers for message.

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
			// Session key
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

		// # Notifygatewayserver
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

		// # Notifyclient
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