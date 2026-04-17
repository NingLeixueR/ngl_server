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
// File overview: Example game manager actor that coordinates matchmaking and room lifecycle.

#pragma once

#include "actor/actor_logic/actor_create/actor_create.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/nsp/nsp_write.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "actor/tab/ttab_specialid.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/protocol/nprotocol.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/generated/pb/example.pb.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	enum eactor_example_manage
	{
		exampleindex_max = pbexample::EPLAY_TYPE_MAX + 1,
		example_waittime = 30,	// Translated comment.
	};

	struct playinfo
	{
		i64_actorid m_actorexampleid = nguid::make();
		pbexample::EPLAY_TYPE m_type = pbexample::EPLAY_NULL;
		int32_t m_createexample = 0;
		std::map<i64_actorid, bool> m_role_enter_example; // Playerwhether instance game
		std::set<i64_actorid> m_roles;
	};

	class actor_example_manage : 
		public actor
	{
		actor_example_manage(const actor_example_manage&) = delete;
		actor_example_manage& operator=(const actor_example_manage&) = delete;

		using type_tuple = std::tuple<pbexample::EPLAY_TYPE, i64_actorid>;

		std::array<int32_t, exampleindex_max> m_exampleindex;
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_info;
		std::map<pbexample::EPLAY_TYPE, std::map<i64_actorid, playinfo>> m_finishinfo;		
		std::map<i64_actorid, type_tuple> m_playerexample; // key:roleid value:actor_exmple_xxx

		actor_example_manage();
	public:
		friend class actor_instance<actor_example_manage>;
		static actor_example_manage& instance()
		{
			return actor_instance<actor_example_manage>::instance();
		}

		virtual ~actor_example_manage() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void erase_actor_before();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		static void nregister();

		void enter_game(playinfo* applayinfo, i64_actorid aroleid, pbexample::ECROSS across, pbexample::EPLAY_TYPE atype);

		bool timer_handle([[maybe_unused]] const message<np_timerparm>& adata);

		bool handle([[maybe_unused]] const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE>>& adata);
		bool handle([[maybe_unused]] const message<np_create_example>& adata);
		bool handle([[maybe_unused]] const message<np_example_equit>& adata);
		bool handle([[maybe_unused]] const message<np_login_request_info>& adata);
	};
}//namespace ngl
