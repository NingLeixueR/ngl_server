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
// File overview: Family/guild actor that manages guild membership, ranks, and guild-wide operations.

#pragma once

#include "actor/actor_logic/actor_events/events_logic.h"
#include "actor/actor_logic/actor_events/actor_events.h"
#include "actor/actor_logic/actor_role/logic/bag/drop.h"
#include "actor/actor_logic/actor_family/family.h"
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
#include "tools/type.h"

namespace ngl
{
	class actor_family : 
		public actor
	{
		actor_family(const actor_family&) = delete;
		actor_family& operator=(const actor_family&) = delete;

		friend class familyer;
		friend class family;
		familyer m_familyer;
		family m_family;
		drop<actor_family> m_drop;

		actor_family();
	public:
		friend class actor_instance<actor_family>;
		static actor_family& instance()
		{
			return actor_instance<actor_family>::instance();
		}

		virtual ~actor_family() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		drop<actor_family>& get_drop()
		{
			return m_drop;
		}
		
		// GM command handler
		using handle_cmd = tools::cmd<actor_mail, std::string, int, ncjson&>;

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<np_gm>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CEDE>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CHANGENAME>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_CREATE>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_INFO>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_JOIN>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_LEAVE>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN>>& adata);
		bool handle([[maybe_unused]] const message<mforward<pbnet::PROBUFF_NET_FAMIL_SIGN>>& adata);
		bool handle([[maybe_unused]] const message<np_eevents_logic_rolelogin>& adata);
		bool handle([[maybe_unused]] const message<np_eevents_logic_roleoffline>& adata);
	};
}// namespace ngl
