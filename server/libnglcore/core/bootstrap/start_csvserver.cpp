#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"


startup_error start_csvserver(int* atcp_port)
{
	return start_node("RELOADCSV", atcp_port, make_node_opt(), []()
		{
			ngl::actor_csvserver::instance();
			ngl::actor_gmclient::instance();
		});
}
