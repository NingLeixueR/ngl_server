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
// 创建时间 // 创建时间 26-01-25 22:05:46
#pragma once

#include "nactortype.h"

#define em_events_null(NAME) (ENUM_ACTOR)(ACTOR_EVENTS+ NAME::id_index())

namespace ngl
{
	struct autoactor
	{
		template <typename TACTOR>
		static void func(ENUM_ACTOR ENUM)
		{
			em<ENUM_ACTOR>::set(ENUM, tools::type_name<TACTOR>().c_str());
			nactor_type<TACTOR>::inits(ENUM);
		}

		template <typename ...ARG>
		static void func(const std::array<ENUM_ACTOR, sizeof ...(ARG)>& aENUMs)
		{
			int32_t lindex = 0;
			(func<ARG>(aENUMs[lindex++]), ...);
		}
	};	
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
		autoactor::func<
			actor_role
			, actor_robot
			, actor_log
			, actor_example_guess_number
			, actor_server
			, actor_client
			, actor_login
			, actor_gateway
			, actor_gateway_c2g
			, actor_gateway_g2c
			, actor_create
			, actor_role_manage
			, actor_kcp
			, actor_robot_manage
			, actor_csvserver
			, actor_csvclient
			, actor_notice
			, actor_gm
			, actor_gmclient
			, actor_mail
			, actor_chat
			, actor_ranklist
			, actor_activity_manage
			, actor_brief
			, actor_keyvalue
			, actor_family
			, actor_friends
			, actor_example_match
			, actor_example_manage
			, actor_testlua
			, actor_testlua2
		>({ACTOR_ROLE, ACTOR_ROBOT, ACTOR_LOG, ACTOR_EXAMPLE_GUESS_NUMBER, ACTOR_SERVER, ACTOR_CLIENT, ACTOR_LOGIN, ACTOR_GATEWAY, ACTOR_GATEWAY_C2G, ACTOR_GATEWAY_G2C, ACTOR_CREATE, ACTOR_ROLE_MANAGE, ACTOR_KCP, ACTOR_ROBOT_MANAGE, ACTOR_CSVSERVER, ACTOR_CSVCLIENT, ACTOR_NOTICE, ACTOR_GM, ACTOR_GMCLIENT, ACTOR_MAIL, ACTOR_CHAT, ACTOR_RANKLIST, ACTOR_ACTIVITY_MANAGE, ACTOR_BRIEF, ACTOR_KEYVALUE, ACTOR_FAMILY, ACTOR_FRIENDS, ACTOR_EXAMPLE_MATCH, ACTOR_EXAMPLE_MANAGE, ACTOR_TESTLUA, ACTOR_TESTLUA2});
	}
}//namespace ngl
