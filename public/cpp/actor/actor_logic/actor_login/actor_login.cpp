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
// File overview: Implements login actor construction and authentication flow.


#include "actor/actor_logic/actor_login/actor_login.h"
#include "actor/actor_base/core/nregister.h"

namespace ngl
{
	actor_login::actor_login() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_LOGIN,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff
			}),
		m_config_area(0)
	{
		m_config_area = tab_self_area;
	}

	ENUM_ACTOR actor_login::actor_type()
	{
		return ACTOR_LOGIN;
	}

	i64_actorid actor_login::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_login::init()
	{
		// Bind the DB structure: DB.set(this);
		m_account.set(this);

		// Set up the timer_handle timer.
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_login::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		ready().set_ready<np_actorserver_connect>("game or gatewa not connect", [this]()
			{
				return !(m_game.empty() || m_gateway.empty());
			}, e_hightlevel_custom
		);
	}

	void actor_login::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_login::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_login::nregister()
	{
		// Timer.
		actor::register_timer<actor_login>(&actor_login::timer_handle);

		// Bind custom np_ messages.
		register_handle<actor_login
			, np_actorserver_connect
		>();
		register_handle<actor_login
			, np_actor_disconnect_close
			, pbnet::PROBUFF_NET_ACOUNT_LOGIN
		>();
	}

	data_modified<pbdb::db_account>* actor_login::get_account(int area, const std::string& account, const std::string& apassworld, bool& aiscreate)
	{
		aiscreate = false;
		auto& lmap = m_account.accountbyarea(area);
		auto lpmodified = tools::findmap(lmap, account);
		if (lpmodified != nullptr)
		{
			MODIFIED_RETURN_CONST(lpdaccountconst, **lpmodified, nullptr);
			if (lpdaccountconst->mpassworld() != apassworld)
			{
				return nullptr;
			}
			return *lpmodified;
		}
		else
		{
			pbdb::db_account laccount;
			laccount.set_maccount(account);
			laccount.set_mpassworld(apassworld);
			i64_actorid lid = nguid::make(ACTOR_ROLE, m_config_area, ++m_account.max_accountid());
			laccount.set_mid(lid);
			laccount.set_mroleid(lid);

			data_modified<pbdb::db_account>& lpaccount = m_account.add(lid, laccount);
			lmap.emplace(account, &lpaccount);
			aiscreate = true;
			return &lpaccount;
		}
	}

	bool actor_login::get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair)
	{
		apair.first = -1;
		apair.second = -1;
		server_info* lpinfo = nullptr;
		for (auto& lpair : amap)
		{
			if (apair.second == -1 || apair.second > lpair.second.m_rolesize)
			{
				apair.first = lpair.second.m_id;
				apair.second = lpair.second.m_rolesize;
				lpinfo = &lpair.second;
			}
		}
		if (lpinfo != nullptr)
		{
			++lpinfo->m_rolesize;
		}
		return apair.first > 0;
	}

	bool actor_login::get_freeserver_game(std::pair<i32_serverid, int32_t>& apair)
	{
		return get_freeserver(m_game, apair);
	}

	bool actor_login::get_freeserver_gateway(std::pair<int32_t, int32_t>& apair)
	{
		return get_freeserver(m_gateway, apair);
	}

	bool actor_login::dec_freeserver(std::map<i32_serverid, server_info>& amap, i32_serverid aserverid)
	{
		auto lpinfo = tools::findmap(amap, aserverid);
		if (lpinfo == nullptr)
		{
			return false;
		}
		--(lpinfo->m_rolesize);
		return true;
	}

	bool actor_login::dec_freeserver_game(i32_serverid aserverid)
	{
		return dec_freeserver(m_game, aserverid);
	}

	bool actor_login::dec_freeserver_gateway(i32_serverid aserverid)
	{
		return dec_freeserver(m_gateway, aserverid);
	}

	void actor_login::printf_freeserver()
	{
		log_error()->print("game[{}] \ngateway[{}]", m_game, m_gateway);
	}

	bool actor_login::timer_handle([[maybe_unused]] const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl
