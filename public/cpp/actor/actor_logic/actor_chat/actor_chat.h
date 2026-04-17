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
// File overview: Chat actor that dispatches messages across channels (world, guild, private).

#pragma once

#include "actor/actor_logic/actor_create/actor_create.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_chat : 
		public actor
	{
		actor_chat(const actor_chat&) = delete;
		actor_chat& operator=(const actor_chat&) = delete;

		std::map<int64_t, int>						m_lastspeakutc;
		std::map<int, std::list<pbnet::chatitem>>	m_chatitem;
		std::map<int, std::list<pbnet::chatitem>>	m_update_chatitem;

		actor_chat();
	public:
		friend class actor_instance<actor_chat>;
		static actor_chat& instance()
		{
			return actor_instance<actor_chat>::instance();
		}

		virtual ~actor_chat() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_CHAT>>& adata);
	};
}//namespace ngl
