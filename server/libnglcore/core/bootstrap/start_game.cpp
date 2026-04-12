// File overview: Implements the bootstrap entry point for game-node startup.
#include "actor/actor_logic/actor_role_manage/actor_role_manage.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/generated/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"


startup_error start_game(int* atcp_port)
{
	return start_node("GAME", atcp_port, make_node_opt({ pbnet::KCP_ROLE }), []()
		{
			ngl::actor_role_manage::instance();
			ngl::actor_create::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}

