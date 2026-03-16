/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor logic.

#pragma once

#include "actor/actor_logic/actor_gateway_c2g/actor_gateway_c2g.h"
#include "actor/actor_logic/actor_gateway_g2c/actor_gateway_g2c.h"
#include "actor/actor_logic/actor_gateway/actor_gateway.h"
#include "actor/actor_logic/actor_robot/actor_robot.h"
#include "actor/actor_logic/actor_role/actor_role.h"
#include "actor/actor_base/actor.h"
#include "tools/template_arg.h"
#include "tools/tab/xml/xml.h"

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
		
		// Secondaryforwarding [client]->[gateway] -> [game] -> [module]
		// ############################## Forwarding ###### secondaryforwarding #######
		// Need tosupportsecondaryforwarding
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