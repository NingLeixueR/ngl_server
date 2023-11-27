#pragma once

#include "dumper.h"
#include "csv.h"
#include "nlog.h"
#include "actor_db.h"
#include "actor_server.h"
#include "actor_db_client.h"
#include "actor_login.h"
#include "actor_gateway.h"
#include "actor_manage_role.h"
#include "actor_robot.h"
#include "actor_create.h"
#include "actor_log.h"
#include "actor_reloadcsv.h"
#include "actor_chat.h"
#include "xmlnode.h"
#include "nlog.h"
#include "splite.h"
#include "robot_cmd.h"
#include "encryption.h"
#include "varint.h"
#include "actor_gateway_game2client.h"
#include "actor_gateway_client2game.h"
#include "uuid.h"
#include "actor_timer.h"
#include "dbredis.h"
#include "db.pb.h"
#include "conversion.h"
#include "time_wheel.h"
#include "attribute.h"
#include "varint.h"
#include "net.pb.h"
#include "init_protobuf.h"
#include "manage_curl.h"
#include "notify.h"
#include "xmlprotocol.h"
#include "actor_notice.h"
#include "actor_gm.h"
#include "actor_mail.h"
#include "actor_manage_activity.h"
#include "actor_brief.h"
#include "rebot_test.h"

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

	ngl::nets::init(tab->m_port, tab->m_threadnum, tab->m_outernet);

	ngl::actor_manage::getInstance().init(tab->m_actorthreadnum);
	LogLocalError("ngl::actor_manage::getInstance().init(%)", tab->m_actorthreadnum);
	return true;
}