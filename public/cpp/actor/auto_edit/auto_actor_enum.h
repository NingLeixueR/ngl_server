/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-10-31 17:15:13
#pragma once

#include "nactortype.h"

#define em_events_null(NAME) null<NAME>,(ENUM_ACTOR)(ACTOR_EVENTS+ NAME::id_index()), #NAME
#define dautoactor(NAME, DEF) null<NAME>, em_pram(DEF)

namespace ngl
{
	template <typename TACTOR>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname)
	{
		em<ENUM_ACTOR>::set(aenum, aname);
		nactor_type<TACTOR>::inits(aenum);
	}

	template <typename TACTOR, typename ...ARG>
	void _auto_actor(const TACTOR* aactor, ENUM_ACTOR aenum, const char* aname, const ARG&... arg)
	{
		_auto_actor<TACTOR>(aactor, aenum, aname);
		_auto_actor(arg...);
	}
}//namespace ngl
#include "actor_role.h"
#include "actor_robot.h"
#include "actor_log.h"
#include "actor_example_guess_number.h"
#include "actor_server.h"
#include "actor_client.h"
#include "actor_login.h"
#include "actor_gateway.h"
#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_create.h"
#include "actor_role_manage.h"
#include "actor_kcp.h"
#include "actor_robot_manage.h"
#include "actor_csvserver.h"
#include "actor_csvclient.h"
#include "actor_notice.h"
#include "actor_gm.h"
#include "actor_gmclient.h"
#include "actor_mail.h"
#include "actor_chat.h"
#include "actor_ranklist.h"
#include "actor_activity_manage.h"
#include "actor_brief.h"
#include "actor_keyvalue.h"
#include "actor_family.h"
#include "actor_friends.h"
#include "actor_example_match.h"
#include "actor_example_manage.h"
#include "actor_testlua.h"
#include "actor_testlua2.h"
namespace ngl
{
	void auto_actor_enum()
	{
		_auto_actor(
			dautoactor(actor_role, ACTOR_ROLE)
			, dautoactor(actor_robot, ACTOR_ROBOT)
			, dautoactor(actor_log, ACTOR_LOG)
			, dautoactor(actor_example_guess_number, ACTOR_EXAMPLE_GUESS_NUMBER)
			, dautoactor(actor_server, ACTOR_SERVER)
			, dautoactor(actor_client, ACTOR_CLIENT)
			, dautoactor(actor_login, ACTOR_LOGIN)
			, dautoactor(actor_gateway, ACTOR_GATEWAY)
			, dautoactor(actor_gateway_c2g, ACTOR_GATEWAY_C2G)
			, dautoactor(actor_gateway_g2c, ACTOR_GATEWAY_G2C)
			, dautoactor(actor_create, ACTOR_CREATE)
			, dautoactor(actor_role_manage, ACTOR_ROLE_MANAGE)
			, dautoactor(actor_kcp, ACTOR_KCP)
			, dautoactor(actor_robot_manage, ACTOR_ROBOT_MANAGE)
			, dautoactor(actor_csvserver, ACTOR_CSVSERVER)
			, dautoactor(actor_csvclient, ACTOR_CSVCLIENT)
			, dautoactor(actor_notice, ACTOR_NOTICE)
			, dautoactor(actor_gm, ACTOR_GM)
			, dautoactor(actor_gmclient, ACTOR_GMCLIENT)
			, dautoactor(actor_mail, ACTOR_MAIL)
			, dautoactor(actor_chat, ACTOR_CHAT)
			, dautoactor(actor_ranklist, ACTOR_RANKLIST)
			, dautoactor(actor_activity_manage, ACTOR_ACTIVITY_MANAGE)
			, dautoactor(actor_brief, ACTOR_BRIEF)
			, dautoactor(actor_keyvalue, ACTOR_KEYVALUE)
			, dautoactor(actor_family, ACTOR_FAMILY)
			, dautoactor(actor_friends, ACTOR_FRIENDS)
			, dautoactor(actor_example_match, ACTOR_EXAMPLE_MATCH)
			, dautoactor(actor_example_manage, ACTOR_EXAMPLE_MANAGE)
			, dautoactor(actor_testlua, ACTOR_TESTLUA)
			, dautoactor(actor_testlua2, ACTOR_TESTLUA2)
		);
	}
}//namespace ngl