#include "actor_login.h"
#include "nregister.h"

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
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_login::loaddb_finish(bool adbishave)
	{
		log_error()->print("actor_login::loaddb_finish");
		for (const auto& item : m_account.data())
		{
			const pbdb::db_account& laccount = item.second.getconst();
			tools::print_protojson(laccount);
		}
	}

	void actor_login::nregister()
	{
		// 定时器
		actor::register_timer<actor_login>(&actor_login::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_login>::func<
			np_actorserver_connect
		>(true);
		register_handle_custom<actor_login>::func<
			np_actor_disconnect_close
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_login>::func<
			pbnet::PROBUFF_NET_ACOUNT_LOGIN
		>(false);
	}

	data_modified<pbdb::db_account>* actor_login::get_account(int area, const std::string& account, const std::string& apassworld, bool& aiscreate)
	{
		aiscreate = false;
		auto& lmap = m_account.accountbyarea(area);
		auto itor = lmap.find(account);
		if (itor != lmap.end())
		{
			if (itor->second->getconst().m_passworld() != apassworld)
			{
				return nullptr;
			}
			return itor->second;
		}
		else
		{
			pbdb::db_account laccount;
			laccount.set_m_account(account);
			laccount.set_m_passworld(apassworld);
			int64_t lid = nguid::make(ACTOR_ROLE, m_config_area, ++m_account.max_accountid());
			laccount.set_m_id(lid);
			laccount.set_m_roleid(lid);

			data_modified<pbdb::db_account>* lpaccount = m_account.add(lid, laccount);
			lmap[account] = lpaccount;
			aiscreate = true;
			return lpaccount;
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
		auto itor = amap.find(aserverid);
		if (itor == amap.end())
		{
			return false;
		}
		--itor->second.m_rolesize;
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

	bool actor_login::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl