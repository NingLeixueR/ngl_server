#pragma once

#include "ndb_modular.h"
#include "nsp_client.h"
#include "nsp_server.h"
#include "actor_db.h"
#include "db.pb.h"

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
		// 订阅/发布[数据副本]
		using nsp_ser	 = nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cli	 = nsp_client<TDerived, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_constcli = nsp_constclient<TDerived, TACTOR, TDBTAB>;

		template <typename TDerived>
		static nsp_constcli<TDerived>& get_constcli()
		{
			static nsp_constcli<TDerived> ltemp(nsp_cli<TDerived>::getInstance());
			return ltemp;
		}

		template <typename TDerived>
		static void constcli_init(TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			nsp_cli<TDerived>::getInstance().init(aactor, adataid);
		}

		// [aregister == true] 主要是注册协议,宏与类型的绑定
		// [aregister == false] 实例化db_actor,db server需要
		static void init(bool aregister);
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	void ndb_modular<ENUM, TDATA, TACTOR>::init_data()
	{
		initdata();
		typedb<ENUM, TDATA, TACTOR>::nsp_ser::loadfish_sync();
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


	class tdb
	{
	public:
		static void tdb_init(bool ainstance);

		// # 定义在nactor_auto.cpp中,因为跨服是可选的
		static void tcrossdb_init(bool ainstance);
	};
}//namespace ngl