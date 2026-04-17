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
// File overview: Log actor that receives log entries from other actors and writes them to persistent storage.

#pragma once

#include "actor/actor_logic/actor_create/actor_create.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "tools/log/logprintf.h"
#include "net/tcp/ntcp.h"

#include <fstream>
#include <memory>

namespace ngl
{
	struct log_timerparm
	{
		enum etype
		{
			e_logflush,
			e_create,
		};
		etype m_type;

		explicit log_timerparm(etype atype) :
			m_type(atype)
		{}
	};

	class actor_log : 
		public actor
	{
		actor_log() = delete;
		actor_log(const actor_log&) = delete;
		actor_log& operator=(const actor_log&) = delete;

		std::map<ELOG_TYPE, std::shared_ptr<logfile>> m_logarr;
	public:
		explicit actor_log(int32_t atype);

		virtual ~actor_log() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(i32_actordataid aid);

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<np_logitem>& adata);
	};
}//namespace ngl
