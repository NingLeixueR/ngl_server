#pragma once

#include "actor_manage_activity.h"
#include "actor_manage_role.h"
#include "actor_gatewayg2c.h"
#include "actor_gatewayc2g.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_calendar.h"
#include "actor_gateway.h"
#include "actor_server.h"
#include "actor_create.h"
#include "actor_notice.h"
#include "actor_brief.h"
#include "actor_login.h"
#include "actor_robot.h"
#include "actor_mail.h"
#include "actor_chat.h"
#include "actor_log.h"
#include "actor_kcp.h"
#include "actor_db.h"
#include "actor_gm.h"
#include "xmlprotocol.h"
#include "manage_curl.h"
#include "encryption.h"
#include "conversion.h"
#include "time_wheel.h"
#include "ndbclient.h"
#include "attribute.h"
#include "tprotocol.h"
#include "sysconfig.h"
#include "xmlnode.h"
#include "dbredis.h"
#include "ntimer.h"
#include "varint.h"
#include "net.pb.h"
#include "notify.h"
#include "splite.h"
#include "dumper.h"
#include "varint.h"
#include "db.pb.h"
#include "uuid.h"
#include "nlog.h"
#include "nlog.h"
#include "csv.h"

#include <chrono>
#include <thread>
#include <string_view>
#include <encryption.h>
#include <cstdint>
#include <string>
#include <vector>

bool init_server(int aid)
{
	ngl::init_nactor_type(true);

	ngl::xmlprotocol::load();

	ngl::nactortype::getInstance();

	ngl::actor_base::start_broadcast();

	ngl::actor_base::init_gatewayser();

	ngl::tab_servers* tab = ngl::ttab_servers::tab();

	ngl::sysconfig::init();

	ngl::nets::init(tab->m_threadnum, tab->m_outernet);

	ngl::events::init();

	ngl::actor_manage::getInstance().init(tab->m_actorthreadnum);

	ngl::np_actor_logitem::m_init = true;
	ngl::log_error()->print("ngl::actor_manage::getInstance().init({})", tab->m_actorthreadnum);

	return true;
}