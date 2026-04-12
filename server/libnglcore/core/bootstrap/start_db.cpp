// File overview: Implements the bootstrap entry point for database-node startup.
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

bool need_seed_db(int aargc, char** aargv)
{
	return aargc >= 5 && aargv != nullptr && aargv[4] != nullptr && std::string_view(aargv[4]) == "init";
}

startup_error start_db(int aargc, char** aargv, int* atcp_port)
{
	return start_node("DB", atcp_port, make_node_opt(), [aargc, aargv]() -> bool
		{
			if (!init_db(nconfig.db()))
			{
				return false;
			}
			ngl::tdb::tdb_init(false);
			ngl::actor_gmclient::instance();

			if (need_seed_db(aargc, aargv))
			{
				init_db_bag();
				init_db_task();
				init_db_role();
				init_db_note();
				init_db_fam();
				init_db_kv();
				init_db_acc();
				init_db_rkv();
				init_db_rank();
				init_db_frd();
				init_db_tlua();
			}
			return true;
		});
}
