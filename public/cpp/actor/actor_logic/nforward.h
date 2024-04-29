#pragma once

#include "actor_gatewayc2g.h"
#include "actor_gatewayg2c.h"
#include "actor_gateway.h"
#include "actor_robot.h"
#include "actor_role.h"
#include "actor.h"
#include "template_arg.h"
#include "xmlnode.h"

namespace ngl
{
	class nforward
	{
		template <EPROTOCOL_TYPE TYPE, typename ...ARG>
		static void register_recvforward()
		{
			if (xmlnode::m_nodetype == ngl::GAME)
			{
				actor_role::type_register_recvforward_handle<TYPE, actor_role>:: template func<ARG...>();
				return;
			}
			if (xmlnode::m_nodetype == ngl::GATEWAY)
			{
				actor_gatewayc2g::type_register_forward_handle<TYPE, false, actor_gatewayc2g>:: template func<ARG...>();
				return;
			}
		}
		
		// 二次转发   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		第一次转发		######	第二次转发	#######
		// 需要支持二次转发
		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename ...ARG>
		static void register_recvforward2()
		{
			if (xmlnode::m_nodetype == ngl::GAME)
			{
				actor_role::type_register_recvforward_handle2<TYPE, ACTOR, actor_role>:: template func<ARG...>();
				return;
			}
			if (xmlnode::m_nodetype == ngl::GATEWAY)
			{
				actor_gatewayc2g::type_register_forward_handle<TYPE, false, actor_gatewayc2g>:: template func<ARG...>();
				return;
			}
		}

		template <EPROTOCOL_TYPE TYPE>
		class register_forward
		{
		public:
			template <typename T>
			static void func()
			{
				
				switch (xmlnode::m_nodetype)
				{
				case ngl::ROBOT:// gateway->client
					actor::type_register_actor_handle<TYPE, actor_robot>::template func<T>(false);
					break;
				case ngl::GATEWAY:// game->gateway
					actor_gatewayg2c::register_forward<TYPE, true, actor_gatewayg2c>(
						(Tfun<actor_gatewayg2c, np_actor_forward<T, TYPE, true, ngl::forward>>) & actor_gatewayg2c::handle
					);
					break;
				}
			}
		};

	public:
		//// ### client->gateway->game
		static void c2g();

		////// ### game->gateway->client
		static void g2c();
	};
}//namespace ngl