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
		// 绑定DB结构:DB.set(this);
		m_account.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_login::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		ready().set_readybycustom([this]()
			{
				return !(m_game.empty() || m_gateway.empty());
			});
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
		// 定时器
		actor::register_timer<actor_login>(&actor_login::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_login
			, np_actorserver_connect
		>(e_ready_null);
		register_handle<actor_login
			, np_actor_disconnect_close
			, pbnet::PROBUFF_NET_ACOUNT_LOGIN
		>(e_ready_all);
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
			lmap[account] = &lpaccount;
			aiscreate = true;
			return &lpaccount;
		}
	}

	bool actor_login::get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair)
	{
		apair.first = -1;
		apair.second = -1;
		server_info* lpinfo = nullptr;
		for (std::pair<const i32_serverid, server_info>& lpair : amap)
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

	bool actor_login::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}
}//namespace ngl