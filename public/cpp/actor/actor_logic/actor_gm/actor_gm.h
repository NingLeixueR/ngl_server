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
// File overview: Declares interfaces for actor gm.

#pragma once

#include "actor/actor_logic/actor_notice/notice.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/protocol/tprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/curl/ncurl.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	class actor_gm : 
		public actor
	{
		actor_gm(const actor_gm&) = delete;
		actor_gm& operator=(const actor_gm&) = delete;

		actor_gm();
	public:
		friend class actor_instance<actor_gm>;
		static actor_gm& instance()
		{
			return actor_instance<actor_gm>::instance();
		}

		virtual ~actor_gm() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i16_area area);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		// # Forwardingtosingleton
		void sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro);

		// # Forwardingto actor
		void sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro);

		static bool checklocalbytype(NODE_TYPE atype);

		void init_handle_cmd();
		
		static bool sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata);

		using handle_cmd = cmd<actor_gm, std::string, ncjson&, const message<ngl::np_gm>*>;

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<ngl::np_gm_response>>& adata);
		bool handle([[maybe_unused]] const message<ngl::np_gm>& adata);
	};
}// namespace ngl
