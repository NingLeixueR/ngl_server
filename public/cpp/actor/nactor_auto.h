#pragma once

#include "ndb_modular.h"
#include "actor_db.h"
#include "define.h"
#include "db.pb.h"

namespace ngl
{
	template <EPROTOCOL_TYPE PROTYPE, pbdb::ENUM_DB TDBTAB_TYPE, typename TDBTAB, typename TACTOR>
	class typedb
	{
	public:
		using db_actor = ngl::actor_db<PROTYPE, TDBTAB_TYPE, TDBTAB>;
		using db_modular = ndb_modular<PROTYPE, TDBTAB_TYPE, TDBTAB, TACTOR>;

		static void init();
		static void instance();
	};

	class actor_login;
	class actor_brief;
	class actor_role;
	class actor_mail;
	class actor_guild;
	class actor_manage_activity;
	class actor_brief;
	class actor_ranklist;
	class actor_calendar;
	class actor_notice;

	using typedb_account = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ACCOUNT, pbdb::db_account, actor_login>;
	using typedb_brief = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_BRIEF, pbdb::db_brief, actor_brief>;
	using typedb_role = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ROLE, pbdb::db_role, actor_role>;
	using typedb_bag = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_BAG, pbdb::db_bag, actor_role>;
	using typedb_task = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_TASK, pbdb::db_task, actor_role>;
	using typedb_rolekv = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_KEYVALUE, pbdb::db_keyvalue, actor_role>;
	using typedb_mail = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_MAIL, pbdb::db_mail, actor_mail>;
	using typedb_guild = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_GUILD, pbdb::db_guild, actor_guild>;
	using typedb_notice = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_NOTICE, pbdb::db_notice, actor_notice>;
	using typedb_activity = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_ACTIVITY, pbdb::db_activity, actor_manage_activity>;
	using typedb_brief = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_BRIEF, pbdb::db_brief, actor_brief>;
	using typedb_ranklist = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_RANKLIST, pbdb::db_ranklist, actor_ranklist>;
	using typedb_calendar = typedb<EPROTOCOL_TYPE_PROTOCOLBUFF, pbdb::ENUM_DB_CALENDAR, pbdb::db_calendar, actor_calendar>;

}//namespace ngl