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
// File overview: Declares interfaces for actor login.

#pragma once

#include "actor/actor_logic/actor_login/account.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_base/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/db/sql/db_data.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"
#include "net/tcp/ntcp.h"

#include <format>

namespace ngl
{
	struct pair_account
	{
		i32_serverid	m_gameserverid = 0;
		i32_serverid	m_gatewayserverid = 0;
		std::string		m_session;
	};
	struct server_info
	{
		int32_t	m_id = 0;
		int32_t	m_rolesize = 0;
	};

	class actor_login : 
		public actor
	{
	public:
	private:
		// ----- Data Begin -----
		account								m_account;
		std::map<i32_serverid, server_info> m_game;
		std::map<i32_serverid, server_info> m_gateway;
		i16_area							m_config_area;
		std::map<i64_actorid, pair_account> m_actorbyserver;
		// ----- Data End   -----

		actor_login(const actor_login&) = delete;
		actor_login& operator=(const actor_login&) = delete;

		actor_login();
	public:
		friend class actor_instance<actor_login>;
		static actor_login& instance() 
		{ 
			return actor_instance<actor_login>::instance(); 
		}

		virtual ~actor_login() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # Account getpbdb::db_account
		data_modified<pbdb::db_account>* get_account(int area, const std::string& account, const std::string& apassworld, bool& aiscreate);

		// # Getamapin server
		bool get_freeserver(std::map<i32_serverid, server_info>& amap, std::pair<i32_serverid, int32_t>& apair);

		// # Getgameserverin server
		bool get_freeserver_game(std::pair<i32_serverid, int32_t>& apair);

		// # Getgatewayserverin server
		bool get_freeserver_gateway(std::pair<int32_t, int32_t>& apair);

		// # Amapinaserveridcorrespondingserver
		bool dec_freeserver(std::map<i32_serverid, server_info>& amap, i32_serverid aserverid);

		// # Gameserverinaserveridcorrespondingserver
		bool dec_freeserver_game(i32_serverid aserverid);

		// # Gatewayserverinaserveridcorrespondingserver
		bool dec_freeserver_gateway(i32_serverid aserverid);

		// # Serverdata
		void printf_freeserver();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_actor_disconnect_close>& adata);
		bool handle(const message<np_actorserver_connect>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ACOUNT_LOGIN>& adata);
	};
}//namespace ngl

template <>
struct std::formatter<ngl::server_info>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const ngl::server_info& aval, std::format_context& ctx)const
	{
		return std::format_to(ctx.out(), "[server_info:id{}-rolesize{}]", aval.m_id, aval.m_rolesize);
	}
};
