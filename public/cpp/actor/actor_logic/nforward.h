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
#pragma once

#include "actor_gateway_c2g.h"
#include "actor_gateway_g2c.h"
#include "actor_gateway.h"
#include "template_arg.h"
#include "actor_robot.h"
#include "actor_role.h"
#include "actor.h"
#include "xml.h"

namespace ngl
{
	class nforward
	{
		template <typename ...ARG>
		static void register_c2g()
		{
			if (ngl::GATEWAY == nconfig.nodetype())
			{
				actor::register_forward_c2g<actor_gateway_c2g, ARG...>();
				return;
			}
			if (ngl::GAME == nconfig.nodetype())
			{
				actor::register_handle<actor_role, ARG...>(e_ready_all);
				return;
			}
		}
		
		// 二次转发   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		第一次转发		######	第二次转发	#######
		// 需要支持二次转发
		template <ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_c2g_2()
		{
			if (ngl::GATEWAY == nconfig.nodetype())
			{
				actor::register_forward_c2g<actor_gateway_c2g, ARG...>();
				return;
			}
			if (ngl::GAME == nconfig.nodetype())
			{
				actor::register_secondary_forward_c2g<actor_role, ACTOR, ARG...>();
				return;
			}
		}

		template <typename ...ARG>
		static void register_g2c()
		{
			if (ngl::GATEWAY == nconfig.nodetype())
			{
				actor::register_forward_g2c<actor_gateway_g2c, ARG...>();
				return;
			}
			if (ngl::ROBOT == nconfig.nodetype())
			{
				actor::register_handle<actor_robot, ARG...>(e_ready_null);
				return;
			}
		}
	public:
		// # client->gateway->game
		static void c2g();

		// # game->gateway->client
		static void g2c();
	};
}//namespace ngl