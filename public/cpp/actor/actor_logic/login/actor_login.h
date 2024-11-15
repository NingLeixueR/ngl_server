#pragma once

#include "actor_manage.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "account.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.pb.h"
#include "db.pb.h"
#include "net.h"
#include "db.h"

#include <format>

namespace ngl
{
	class actor_login : public actor
	{
	public:
		struct server_info
		{
			int32_t	m_id = 0;
			int32_t	m_rolesize = 0;
		};
	private:
		// ----- Data Begin -----
		account								m_account;
		std::map<i32_serverid, server_info> m_game;
		std::map<i32_serverid, server_info> m_gateway;
		i16_area							m_config_area;
		struct pair_account
		{
			i32_serverid	m_gameserverid;
			i32_serverid	m_gatewayserverid;
			std::string		m_session;
		};
		std::map<i64_actorid, pair_account> m_actorbyserver;
		// ----- Data End   -----

		actor_login(const actor_login&) = delete;
		actor_login& operator=(const actor_login&) = delete;

		actor_login();
	public:
		friend class actor_instance<actor_login>;
		static actor_login& getInstance() 
		{ 
			return actor_instance<actor_login>::instance(); 
		}

		virtual void init();

		virtual ~actor_login();

		static void nregister();

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_LOGIN;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_LOGIN, tab_self_area, nguid::none_actordataid());
		}

		virtual void loaddb_finish(bool adbishave);

		// # 根据账号密码获取pbdb::db_account
		data_modified<pbdb::db_account>* get_account(
			int area, const std::string& account, const std::string& apassworld, bool& aiscreate
		);

		// # 获取amap中相对空闲的服务器
		bool get_freeserver(
			std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair
		);

		// # 获取game服务器中相对空闲的服务器
		bool get_freeserver_game(std::pair<i32_serverid, int32_t>& apair);

		// # 获取gateway服务器中相对空闲的服务器
		bool get_freeserver_gateway(std::pair<int32_t, int32_t>& apair);

		// # 减少amap中aserverid对应服务器的承载人数
		bool dec_freeserver(
			std::map<i32_serverid, server_info>& amap, i32_serverid aserverid
		);

		// # 减少game服务器中aserverid对应服务器的承载人数
		bool dec_freeserver_game(i32_serverid aserverid);

		// # 减少gateway服务器中aserverid对应服务器的承载人数
		bool dec_freeserver_gateway(i32_serverid aserverid);

		// # 打印空闲服务器数据
		void printf_freeserver();

		bool handle(const message<np_actorserver_connect>& adata);

		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata);

		bool handle(const message<np_actor_disconnect_close>& adata);
	};
}//namespace ngl

template <>
struct std::formatter<ngl::actor_login::server_info>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const ngl::actor_login::server_info& aval, std::format_context& ctx)const
	{
		return std::format_to(
			ctx.out(), "[server_info:id{}-rolesize{}]", aval.m_id, aval.m_rolesize
		);
	}
};

