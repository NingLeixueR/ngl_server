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
// File overview: Declares interfaces for actor kcp.

#pragma once

#include "actor/actor_logic/actor_role/actor_role.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "tools/db/sql/db_data.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_kcp : 
		public actor
	{
		actor_kcp(const actor_kcp&) = delete;
		actor_kcp& operator=(const actor_kcp&) = delete;

		actor_kcp();
	public:
		friend class actor_instance<actor_kcp>;
		static actor_kcp& instance()
		{
			return actor_instance<actor_kcp>::instance();
		}

		virtual ~actor_kcp() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t aserid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<np_actor_kcp>& adata);
	};
}//namespace ngl
