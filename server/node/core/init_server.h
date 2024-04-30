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
#include "rebot_test.h"
#include "encryption.h"
#include "conversion.h"
#include "time_wheel.h"
#include "robot_cmd.h"
#include "ndbclient.h"
#include "attribute.h"
#include "initproto.h"
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
#include "nlog.h"
#include "uuid.h"
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
	ngl::tab_servers* tab = ngl::ttab_servers::tab();
	bool lvarint = false;
	std::string lxor;

	ngl::xmlinfo* lpublicxml = ngl::xmlnode::get_publicconfig();

	if (!lpublicxml->find("robot_test", ngl::rebot_test::is_test))
		return false;

	// ### 是否开启varint
	if (!lpublicxml->varint(lvarint))
		return false;
	ngl::varint::set(lvarint);

	// 设置简单加密
	bool lisxor = false;
	if (!lpublicxml->isxor(lisxor))
		return false;
	if (!lpublicxml->xor_str(lxor))
		return false;
	ngl::encryption::set_xor(lisxor, lxor.c_str(), lxor.size());

	if (!lpublicxml->find("log_iswrite", ngl::nlog::getInstance().iswrite()))
		return false;

	ngl::nets::init(tab->m_threadnum, tab->m_outernet);

	ngl::actor_manage::getInstance().init(tab->m_actorthreadnum);
	LogLocalError("ngl::actor_manage::getInstance().init(%)", tab->m_actorthreadnum);

	ngl::init_event();
	return true;
}