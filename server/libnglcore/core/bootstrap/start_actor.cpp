#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"


startup_error start_actor(int* atcp_port)
{
	return start_node("ACTORSERVER", atcp_port, make_node_opt({}, false, false, false), []()
		{
			ngl::actor_server::instance();
			make_log_actor();
			ngl::actor_gmclient::instance();
		});
}
