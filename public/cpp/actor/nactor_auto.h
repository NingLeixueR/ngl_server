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
		// ����/����[���ݸ���]
		using nsp_ser	 = nsp_server<TDBTAB_TYPE, TACTOR, TDBTAB>;
		template <typename TDerived>
		using nsp_cli	 = nsp_client<TDerived, TDBTAB>;
		// [aregister == true] ��Ҫ��ע��Э��,�������͵İ�
		// [aregister == false] ʵ����db_actor,db server��Ҫ
		static void init(bool aregister);
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	void ndb_modular<ENUM, TDATA, TACTOR>::init_data()
	{
		initdata();
		typedb<ENUM, TDATA, TACTOR>::nsp_ser::loadfish_sync();
	}

	class actor_login;
	using tdb_account = typedb<pbdb::ENUM_DB_ACCOUNT, pbdb::db_account, actor_login>;

	class actor_brief;
	using tdb_brief = typedb<pbdb::ENUM_DB_BRIEF, pbdb::db_brief, actor_brief>;

	class actor_role;
	using tdb_role		= typedb<pbdb::ENUM_DB_ROLE, pbdb::db_role, actor_role>;
	using tdb_bag		= typedb<pbdb::ENUM_DB_BAG, pbdb::db_bag, actor_role>;
	using tdb_task		= typedb<pbdb::ENUM_DB_TASK, pbdb::db_task, actor_role>;
	using tdb_rolekv	= typedb<pbdb::ENUM_DB_ROLEKEYVALUE, pbdb::db_rolekeyvalue, actor_role>;

	class actor_mail;
	using tdb_mail = typedb<pbdb::ENUM_DB_MAIL, pbdb::db_mail, actor_mail>;

	class actor_family;
	using tdb_family = typedb<pbdb::ENUM_DB_FAMILY, pbdb::db_family, actor_family>;
	using tdb_familyer = typedb<pbdb::ENUM_DB_FAMILYER, pbdb::db_familyer, actor_family>;

	class actor_guild;
	using tdb_guild = typedb<pbdb::ENUM_DB_GUILD, pbdb::db_guild, actor_guild>;

	class actor_notice;
	using tdb_notice = typedb<pbdb::ENUM_DB_NOTICE, pbdb::db_notice, actor_notice>;

	class actor_manage_activity;
	using tdb_activity = typedb<pbdb::ENUM_DB_ACTIVITY, pbdb::db_activity, actor_manage_activity>;
	
	class actor_ranklist;
	using tdb_ranklist = typedb<pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist, actor_ranklist>;

	class actor_calendar;
	using tdb_calendar = typedb<pbdb::ENUM_DB_CALENDAR, pbdb::db_calendar, actor_calendar>;

	class actor_keyvalue;
	using tdb_keyvalue = typedb<pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue, actor_keyvalue>;

	class actor_friends;
	using tdb_friends = typedb<pbdb::ENUM_DB_FRIENDS, pbdb::db_friends, actor_friends>;
}//namespace ngl