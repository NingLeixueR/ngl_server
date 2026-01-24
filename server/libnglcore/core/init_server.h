#pragma once
#include "actor_activity_manage.h"
#include "actor_example_manage.h"
#include "actor_example_match.h"
#include "actor_robot_manage.h"
#include "actor_role_manage.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway_c2g.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_gmclient.h"
#include "actor_keyvalue.h"
#include "actor_ranklist.h"
#include "actor_testlua2.h"
#include "actor_testlua.h"
#include "actor_friends.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "actor_create.h"
#include "actor_notice.h"
#include "actor_family.h"
#include "actor_events.h"
#include "actor_brief.h"
#include "actor_login.h"
#include "actor_robot.h"
#include "xmlprotocol.h"
#include "time_wheel.h"
#include "actor_mail.h"
#include "actor_chat.h"
#include "ndbclient.h"
#include "attribute.h"
#include "tprotocol.h"
#include "sysconfig.h"
#include "actor_log.h"
#include "actor_kcp.h"
#include "db_manage.h"
#include "actor_db.h"
#include "actor_gm.h"
#include "dbredis.h"
#include "ntimer.h"
#include "net.pb.h"
#include "ncurl.h"
#include "db.pb.h"
#include "nlog.h"
#include "nkcp.h"
#include "csv.h"
#include "xml.h"

#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

bool init_server(int aid, const std::set<pbnet::ENUM_KCP>& akcp = {})
{
	// # 加载并关联协议号
	ngl::xmlprotocol::load();

	ngl::auto_actor();
	ngl::tprotocol_customs();
	ngl::tprotocol_forward_pb();
	ngl::event_register();
	ngl::tdb::tdb_init(true);

	// # 启动actor广播
	ngl::actor_base::start_broadcast();

	// # sysconfig关联xml配置
	ngl::sysconfig::init();

	const ngl::tab_servers* tab = ngl::ttab_servers::instance().const_tab();
	if (tab == nullptr)
	{
		ngl::tools::no_core_dump();
		return false;
	}

	// # 启动网络监听
	{//TCP
		ngl::net_works lnwork;
		if (!ngl::ttab_servers::instance().get_nworks(ngl::ENET_PROTOCOL::ENET_TCP, nconfig.tcount(), lnwork))
		{
			ngl::tools::no_core_dump();
			return false;
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

	// # 初始化actor管理模块
	ngl::actor_manage::instance().init(tab->m_actorthreadnum);

	// # actor管理模块已初始化完毕，可以将日志发送给actor_log
	ngl::nactor_logitem::m_init = true;

	ngl::log_error()->print("ngl::actor_manage::instance().init({})", tab->m_actorthreadnum);
	return true;
}