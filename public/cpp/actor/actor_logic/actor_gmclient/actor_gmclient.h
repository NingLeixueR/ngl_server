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
// File overview: Declares interfaces for actor gmclient.

#pragma once

#include "actor/actor_logic/actor_notice/notice.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/actor_manage.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/protocol/tprotocol.h"
#include "actor/actor_base/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/curl/ncurl.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	struct protocols
	{
		std::map<i32_protocolnum, std::string> m_data;

		DPROTOCOL(protocols, m_data)
	};

	class actor_gmclient : 
		public actor
	{
		actor_gmclient(const actor_gmclient&) = delete;
		actor_gmclient& operator=(const actor_gmclient&) = delete;

		actor_gmclient();
	public:
		friend class actor_instance<actor_gmclient>;
		static actor_gmclient& instance()
		{
			return actor_instance<actor_gmclient>::instance();
		}

		virtual ~actor_gmclient() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		static void allprotocol(protocols& apro);

		// # GM
		using handle_cmd = cmd<actor_gmclient, std::string, int, ncjson&>;

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<np_gm>>& adata);
	};
}// namespace ngl