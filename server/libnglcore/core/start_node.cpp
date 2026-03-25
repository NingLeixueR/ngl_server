#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/actor_logic/actor_role_manage/actor_role_manage.h"
#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_logic/actor_csvclient/actor_csvclient.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_testlua2/actor_testlua2.h"
#include "actor/actor_logic/actor_testlua/actor_testlua.h"
#include "actor/actor_logic/actor_friends/actor_friends.h"
#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_logic/actor_server/actor_server.h"
#include "actor/actor_logic/actor_create/actor_create.h"
#include "actor/actor_logic/actor_notice/actor_notice.h"
#include "actor/actor_logic/actor_family/actor_family.h"
#include "actor/actor_logic/actor_events/actor_events.h"
#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/actor_logic/actor_login/actor_login.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/actor_logic/actor_log/actor_log.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/actor_logic/actor_gm/actor_gm.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
#include "actor/actor_base/ndbclient.h"
#include "actor/actor_base/actor_db.h"
#include "tools/tab/xml/xmlprotocol.h"
#include "actor/protocol/tprotocol.h"
#include "actor/actor_base/ntimer.h"
#include "tools/tab/xml/sysconfig.h"
#include "tools/db/redis/dbredis.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xml.h"
#include "tools/time_wheel.h"
#include "net/udp/kcp/nkcp.h"
#include "tools/curl/ncurl.h"
#include "actor/pb/net.pb.h"
#include "actor/pb/db.pb.h"
#include "tools/log/nlog.h"
#include "net/tcp/ws/nws.h"
#include "server_main.h"
#include "start_node.h"

void make_log_actor()
{
	if (ngl::sysconfig::logwritelevel() < ngl::ELOG_MAX)
	{
		int32_t llogtype = ngl::ELOG_DEFAULT;
		ngl::actor_base::create(ngl::ACTOR_LOG, tab_self_area, nconfig.nodeid(), (void*)&llogtype);
	}
}

node_boot_opt make_node_opt(
	std::initializer_list<pbnet::ENUM_KCP> akcp_types,
	bool ause_actor,
	bool amake_log,
	bool areg_actor
)
{
	node_boot_opt lopts;
	lopts.m_kcp_types.insert(akcp_types.begin(), akcp_types.end());
	lopts.m_use_actor_client = ause_actor;
	lopts.m_make_log = amake_log;
	lopts.m_reg_actor = areg_actor;
	return lopts;
}
