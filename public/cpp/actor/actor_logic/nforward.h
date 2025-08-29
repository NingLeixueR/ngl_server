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
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_c2g::register_forward_gateway_c2g<actor_gateway_c2g>:: template func<ARG...>();
				return;
			}
			if (ngl::GAME == xmlnode::m_nodetype)
			{
				actor_role::register_forward_game_c2g<actor_role, ACTOR_ROLE>:: template func<ARG...>();
				return;
			}
		}
		
		// 二次转发   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		第一次转发		######	第二次转发	#######
		// 需要支持二次转发
		template <ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_c2g_2()
		{
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_c2g::register_forward_gateway_c2g<actor_gateway_c2g>::template func<ARG...>();
				return;
			}
			if (ngl::GAME == xmlnode::m_nodetype)
			{
				actor_role::register_forward_game_c2g<actor_role, ACTOR>::template func<ARG...>();
				return;
			}
		}

		template <typename ...ARG>
		static void register_g2c()
		{
			if (ngl::GATEWAY == xmlnode::m_nodetype)
			{
				actor_gateway_g2c::register_forward_gateway_g2c<actor_gateway_g2c>::template func<ARG...>();
				return;
			}
			if (ngl::ROBOT == xmlnode::m_nodetype)
			{
				actor_robot::register_forward_client_g2c<actor_robot>::template func<ARG...>();
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