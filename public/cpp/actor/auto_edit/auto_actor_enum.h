// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-05-29 22:00:41
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
#include "actor_drop.h"
#include "actor_example_match.h"
#include "actor_example_manage.h"
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
			, dautoactor(actor_drop, ACTOR_DROP)
			, dautoactor(actor_example_match, ACTOR_EXAMPLE_MATCH)
			, dautoactor(actor_example_manage, ACTOR_EXAMPLE_MANAGE)
		);
	}
}//namespace ngl