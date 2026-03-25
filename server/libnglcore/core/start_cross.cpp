#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"


startup_error start_cross(int* atcp_port)
{
	return start_node("CROSS", atcp_port, make_node_opt(), []()
		{
			ngl::actor_chat::instance();
			ngl::actor_ranklist::instance();
			ngl::actor_gmclient::instance();
		});
}
