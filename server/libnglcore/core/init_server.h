// File overview: Declares interfaces for core.

#pragma once
#include "actor/actor_logic/actor_activity_manage/actor_activity_manage.h"
#include "actor/actor_logic/actor_example_manage/actor_example_manage.h"
#include "actor/actor_logic/actor_example_match/actor_example_match.h"
#include "actor/actor_logic/actor_robot_manage/actor_robot_manage.h"
#include "actor/actor_logic/actor_role_manage/actor_role_manage.h"
#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_csvserver/actor_csvserver.h"
#include "actor/actor_logic/actor_csvclient/actor_csvclient.h"
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
#include "actor/actor_logic/actor_ranklist/actor_ranklist.h"
#include "actor/actor_logic/actor_testlua2/actor_testlua2.h"
#include "tools/db/sql/mysql/nmysql_manage.h"
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
#include "tools/tab/xml/xmlprotocol.h"
#include "tools/time_wheel.h"
#include "actor/actor_logic/actor_mail/actor_mail.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/actor_base/ndbclient.h"
#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/protocol/tprotocol.h"
#include "tools/tab/xml/sysconfig.h"
#include "actor/actor_logic/actor_log/actor_log.h"
#include "actor/actor_logic/actor_kcp/actor_kcp.h"
#include "actor/actor_base/actor_db.h"
#include "actor/actor_logic/actor_gm/actor_gm.h"
#include "tools/db/redis/dbredis.h"
#include "actor/actor_base/ntimer.h"
#include "actor/pb/net.pb.h"
#include "tools/curl/ncurl.h"
#include "actor/pb/db.pb.h"
#include "tools/log/nlog.h"
#include "net/udp/kcp/nkcp.h"
#include "tools/tab/csv/csv.h"
#include "tools/tab/xml/xml.h"
#include "server_main.h"

#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

startup_error init_server(int aid, const std::set<pbnet::ENUM_KCP>& akcp = {}, int* atcp_port = nullptr)
{
	if (atcp_port != nullptr)
	{
		*atcp_port = -1;
	}

	// # Loadand protocol id
	ngl::xmlprotocol::load();

	ngl::auto_actor();
	ngl::tprotocol_customs();
	ngl::tprotocol_forward_pb();
	ngl::event_register();
	ngl::tdb::tdb_init(true);

	// # Actorbroadcast
	ngl::actor_base::start_broadcast();

	// # Sysconfig xmlconfig
	ngl::sysconfig::init();

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab();
	if (tab == nullptr)
	{
		ngl::tools::no_core_dump();
		return startup_error::tab_server_missing;
	}

	// Translated comment.
	{//TCP
		ngl::net_works lnwork;
		if (!ngl::ttab_servers::instance().get_nworks(ngl::ENET_PROTOCOL::ENET_TCP, nconfig.tcount(), lnwork))
		{
			ngl::tools::no_core_dump();
			return startup_error::net_config_missing;
		}
		if (atcp_port != nullptr)
		{
			*atcp_port = lnwork.m_port;
		}
		ngl::ntcp::instance().init(lnwork.m_port, tab->m_threadnum, tab->m_outernet);
	}

	{//KCP
		if (!akcp.empty())
		{
			for (pbnet::ENUM_KCP kcptype : akcp)
			{
				ngl::nkcp::instance().create_kcp(kcptype);
			}
		}
	}

	// # Initializeactormanagemodule
	ngl::actor_manage::instance().init(tab->m_actorthreadnum);

	// # Actormanagemodule initialize, can logsendtoactor_log
	ngl::nactor_logitem::m_init = true;

	ngl::log_error()->print("ngl::actor_manage::instance().init({})", tab->m_actorthreadnum);
	return startup_error::ok;
}