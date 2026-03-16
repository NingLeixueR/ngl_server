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
// File overview: Declares interfaces for auto.


#pragma once

#include "actor/actor_base/nsp/nsp_server_function.h"
#include "actor/actor_base/nsp/nsp_write_function.h"
#include "actor/actor_base/nsp/nsp_read_function.h"
#include "actor/actor_base/ndb_modular.h"
#include "actor/actor_base/actor_db.h"
#include "actor/pb/db.pb.h"

namespace ngl
{
	template <pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB, typename TACTOR>
	class typedb
	{
		typedb() = delete;
		typedb(const typedb&) = delete;
		typedb& operator=(const typedb&) = delete;
	public:
		using db_actor   = ngl::actor_db<TDBTAB_TYPE, TDBTAB>;
		using db_modular = ndb_modular<TDBTAB_TYPE, TDBTAB, TACTOR>;
		// / [Data ]
		using nsp_ser		= nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cread		= nsp_read<TDerived, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cwrite	= nsp_write<TDerived, TACTOR, TDBTAB>;

		// [Aregister == true] registerprotocol, andtype bind
		// [Aregister == false] instance db_actor,db serverneed to
		static void init(bool aregister);
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	void ndb_modular<ENUM, TDATA, TACTOR>::init_data()
	{
		initdata();
	}


	class  actor_activity_manage;
	class  actor_brief;
	class  actor_cross_test;
	class  actor_family;
	class  actor_friends;
	class  actor_keyvalue;
	class  actor_login;
	class  actor_mail;
	class  actor_notice;
	class  actor_ranklist;
	class  actor_role;
	class  actor_testlua;


	using tdbcross_test = typedb<pbdb::ENUM_DBCROSS_TEST, pbdb::dbcross_test, actor_cross_test>;

	using tdb_account = typedb<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account, actor_login>;
	using tdb_activity = typedb<pbdb::ENUM_DB_ACTIVITY, pbdb::db_activity, actor_activity_manage>;
	using tdb_activitytimes = typedb<pbdb::ENUM_DB_ACTIVITYTIMES, pbdb::db_activitytimes, actor_activity_manage>;
	using tdb_bag = typedb<pbdb::ENUM_DB_BAG, pbdb::db_bag, actor_role>;
	using tdb_brief = typedb<pbdb::ENUM_DB_BRIEF, pbdb::db_brief, actor_brief>;
	using tdb_family = typedb<pbdb::ENUM_DB_FAMILY, pbdb::db_family, actor_family>;
	using tdb_familyer = typedb<pbdb::ENUM_DB_FAMILYER, pbdb::db_familyer, actor_family>;
	using tdb_friends = typedb<pbdb::ENUM_DB_FRIENDS, pbdb::db_friends, actor_friends>;
	using tdb_keyvalue = typedb<pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue, actor_keyvalue>;
	using tdb_mail = typedb<pbdb::ENUM_DB_MAIL, pbdb::db_mail, actor_mail>;
	using tdb_notice = typedb<pbdb::ENUM_DB_NOTICE, pbdb::db_notice, actor_notice>;
	using tdb_ranklist = typedb<pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist, actor_ranklist>;
	using tdb_role = typedb<pbdb::ENUM_DB_ROLE, pbdb::db_role, actor_role>;
	using tdb_rolekv = typedb<pbdb::ENUM_DB_ROLEKEYVALUE, pbdb::db_rolekeyvalue, actor_role>;
	using tdb_task = typedb<pbdb::ENUM_DB_TASK, pbdb::db_task, actor_role>;
	using tdb_testlua = typedb<pbdb::ENUM_DB_TESTLUA, pbdb::db_testlua, actor_testlua>;


	class tdb
	{
	public:
		static void tdb_init(bool ainstance);

		// # Nactor_auto.cppin,becausecross-server
		static void tcrossdb_init(bool ainstance);
	};
}//namespace ngl