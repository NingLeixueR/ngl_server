#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/auto/nactor_auto.h"
#include "tools/tab/xml/xml.h"
#include "init_dbdata.h"
#include "start_node.h"
#include "start_db.h"

startup_error start_gateway(int* atcp_port)
{
	return start_node("GATEWAY", atcp_port, make_node_opt({ pbnet::KCP_GATEWAY }), []()
		{
			ngl::actor_gateway::instance();
			ngl::actor_gateway_g2c::instance();
			ngl::actor_gateway_c2g::instance();
			ngl::actor_gmclient::instance();
			ngl::actor_kcp::instance();
		});
}
