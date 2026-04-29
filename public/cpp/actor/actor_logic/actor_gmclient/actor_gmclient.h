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
// File overview: GM client actor that relays GM commands from an external tool to the GM actor.

#pragma once

#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_logic/actor_notice/notice.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/actor_base/core/ndbclient.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/serialize/socket_pool.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/protocol/tprotocol.h"
#include "tools/tools/tools_curl.h"
#include "tools/db/sql/db_data.h"
#include "tools/tab/csv/ncsv.h"
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

		struct bucket_info
		{
			int32_t bucket_size = 0;           // Bucket size in bytes (64, 128, 256, etc.)
			int32_t bucket_capacity = 0;       // Configured capacity (100, 80, 60, etc.)
			uint64_t current_allocated = 0;    // Currently held externally
			uint64_t peak_allocated = 0;       // Historical peak allocation
			uint64_t current_cached = 0;       // Currently in pool cache

			DPROTOCOL(bucket_info, bucket_size, bucket_capacity, current_allocated, peak_allocated, current_cached)
		};

		struct socket_pool_stats
		{
			uint64_t alloc_count = 0;
			uint64_t free_count = 0;
			uint64_t cache_hit = 0;
			uint64_t cache_miss = 0;
			uint64_t large_alloc = 0;
			uint64_t alloc_fail = 0;
			double hit_rate = 0.0;
			std::vector<bucket_info> buckets;

			DPROTOCOL(socket_pool_stats, alloc_count, free_count, cache_hit, cache_miss, large_alloc, alloc_fail, hit_rate, buckets)
		};

		static socket_pool_stats get_socket_pool_stats();

		// GM command handler type alias.
		using handle_cmd = tools::cmd<actor_gmclient, std::string, int, ncjson&>;

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<np_gm>>& adata);
	};
}// namespace ngl
