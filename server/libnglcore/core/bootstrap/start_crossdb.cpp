#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"


startup_error start_crossdb(int* atcp_port)
{
	return start_node("CROSSDB", atcp_port, make_node_opt(), []() -> bool
		{
			if (!init_db(nconfig.crossdb()))
			{
				return false;
			}
			ngl::tdb::tcrossdb_init(false);
			ngl::actor_gmclient::instance();
			return true;
		});
}
