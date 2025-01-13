#pragma once

#include "actor_manage_activity.h"
#include "actor_manage_role.h"
#include "actor_gatewayg2c.h"
#include "actor_gatewayc2g.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_gmclient.h"
#include "actor_keyvalue.h"
#include "actor_calendar.h"
#include "actor_ranklist.h"
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
#include "manage_curl.h"
#include "time_wheel.h"
#include "actor_mail.h"
#include "actor_chat.h"
#include "ndbclient.h"
#include "attribute.h"
#include "tprotocol.h"
#include "sysconfig.h"
#include "actor_log.h"
#include "actor_kcp.h"
#include "actor_db.h"
#include "actor_gm.h"
#include "dbredis.h"
#include "ntimer.h"
#include "net.pb.h"
#include "dumper.h"
#include "db.pb.h"
#include "nlog.h"
#include "nlog.h"
#include "csv.h"
#include "xml.h"

#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

bool init_server(int aid)
{
	// # 加载并关联协议号
	ngl::xmlprotocol::load();

	ngl::auto_actor();
	ngl::tprotocol_customs();
	ngl::tprotocol_forward_pb();
	ngl::event_register();
	ngl::tdb_init(true);

	// # 启动actor广播
	ngl::actor_base::start_broadcast();

	// # sysconfig关联xml配置
	ngl::sysconfig::init();

	// # 启动网络监听
	const ngl::tab_servers* tab = ngl::ttab_servers::tab();
	ngl::nets::init(tab->m_threadnum, tab->m_outernet);

	// # 初始化actor管理模块
	ngl::actor_manage::getInstance().init(tab->m_actorthreadnum);

	// # actor管理模块已初始化完毕，可以将日志发送给actor_log
	ngl::np_actor_logitem::m_init = true;

	ngl::log_error()->print("ngl::actor_manage::getInstance().init({})", tab->m_actorthreadnum);
	return true;
}