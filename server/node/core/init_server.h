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
	ngl::init_nactor_type(true);

	// # 加载并关联协议号
	ngl::xmlprotocol::load();

	ngl::actor_base::start_broadcast();

	const ngl::tab_servers* tab = ngl::ttab_servers::tab();

	ngl::sysconfig::init();

	ngl::nets::init(tab->m_threadnum, tab->m_outernet);

	ngl::events::init();

	ngl::actor_manage::getInstance().init(tab->m_actorthreadnum);

	ngl::np_actor_logitem::m_init = true;

	ngl::log_error()->print("ngl::actor_manage::getInstance().init({})", tab->m_actorthreadnum);

	return true;
}