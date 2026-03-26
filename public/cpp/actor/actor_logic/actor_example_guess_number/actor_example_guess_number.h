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
// File overview: Declares interfaces for actor example guess number.

#pragma once

#include "actor/actor_logic/actor_example_guess_number/actor_example.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "tools/db/sql/mysql/nmysql_pool.h"
#include "actor/actor_base/core/actor_manage.h"
#include "actor/actor_base/core/ndb_modular.h"
#include "actor/actor_base/core/ndbclient.h"
#include "actor/protocol/nprotocol.h"
#include "actor/tab/ttab_specialid.h"
#include "tools/db/sql/mysql/nmysql.h"
#include "actor/actor_base/core/ntimer.h"
#include "tools/db/sql/db_data.h"
#include "actor/generated/pb/example.pb.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/generated/pb/net.pb.h"
#include "net/tcp/ntcp.h"

namespace ngl
{
	enum eactor_example_guess_number
	{
		guess_number_esetp_maxtime = 30, // Execute
		guess_number_equit_time = 10,	// Exit
		guess_number_max_number = 101,
		guess_number_min_number = 0,
	};

	class actor_example_guess_number : 
		public actor_example<actor_example_guess_number>
	{
		actor_example_guess_number(const actor_example_guess_number&) = delete;
		actor_example_guess_number& operator=(const actor_example_guess_number&) = delete;

		int32_t m_maxnumber		= 1000;	// Translated comment.
		int32_t m_minnumber		= 0;	// Translated comment.
		int32_t m_index			= 0;	// Player(index)
		int32_t m_setputc		= 0;	// Time
		int32_t m_bombvalues	= 0;	// Translated comment.
		int32_t m_bombutc		= 0;	// Time
		i64_actorid m_bombrole	= 0;	// Player
	public:
		actor_example_guess_number(const std::map<int32_t, i64_actorid>& aroleids, int32_t aindex);

		virtual ~actor_example_guess_number() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat);

		void next_setp();

		void bomb(i64_actorid aroleid = nguid::make());

		bool isfinish();

		void set_finish(i64_actorid abombrole);

		i64_actorid next_guess_role();
		i64_actorid cur_guess_role();

		void send_error(i64_actorid aroleid, pbexample::E_GUESS_NUMBER_ERROR aerr);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata);
		bool handle(const message<np_example_entergame_ready>& adata);
	};
}//namespace ngl
