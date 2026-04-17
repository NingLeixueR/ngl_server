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
// File overview: Role manager actor that tracks online player roles and coordinates role lifecycle.

#pragma once

#include "actor/actor_logic/actor_role/actor_role.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/generated/pb/net.pb.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_role_manage : 
		public actor
	{
		actor_role_manage(const actor_role_manage&) = delete;
		actor_role_manage& operator=(const actor_role_manage&) = delete;

		actor_role_manage();

		std::set<i64_actorid> m_roleban;
	public:
		friend class actor_instance<actor_role_manage>;
		static actor_role_manage& instance()
		{ 
			return actor_instance<actor_role_manage>::instance();
		}

		virtual ~actor_role_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// Post-processing hook called after each message is handled.
		virtual void handle_after(handle_pram&);

		// GM command handler type alias.
		using handle_gm = tools::cmd<actor_role, std::string, int, ncjson&>;

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<np_gm>>& adata);
		bool handle([[maybe_unused]] const message<pbnet::PROBUFF_NET_ROLE_LOGIN>& adata);
	};
}//namespace ngl
