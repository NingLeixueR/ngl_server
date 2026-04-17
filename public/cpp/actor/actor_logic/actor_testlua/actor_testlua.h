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
// File overview: Test actor for validating Lua scripting integration.

#pragma once

#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_testlua/testlua.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/log/nlog.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_testlua :
		public actor
	{
		actor_testlua(const actor_testlua&) = delete;
		actor_testlua& operator=(const actor_testlua&) = delete;

		testlua m_testlua;

		actor_testlua();
	public:
		friend class actor_instance<actor_testlua>;
		static actor_testlua& instance()
		{
			return actor_instance<actor_testlua>::instance();
		}
		virtual ~actor_testlua() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_TESTLUA>>& adata);
	};
}// namespace ngl
