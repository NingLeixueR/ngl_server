// File overview: Implements the bootstrap entry point for login-node startup.
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_login/actor_login.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

startup_error start_login(int* atcp_port)
{
	return start_node("LOGIN", atcp_port, make_node_opt(), []()
		{
			ngl::actor_login::instance();
			ngl::actor_gmclient::instance();
		});
}
