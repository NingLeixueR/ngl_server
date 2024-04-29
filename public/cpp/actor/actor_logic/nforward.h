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
		template <EPROTOCOL_TYPE TYPE>
		class register_recvforward
		{
		public:
			template <typename T>
			static void func()
			{
				switch (xmlnode::m_nodetype)
				{
				case ngl::GAME:// gateway->game
					actor_role::register_recvforward<TYPE, actor_role>(dregister_fun_handle(actor_role, T));
					break;
				case ngl::GATEWAY:// client->gateway
					actor_gatewayc2g::register_forward<TYPE, false, actor_gatewayc2g>(
						(Tfun<actor_gatewayc2g, np_actor_forward<T, TYPE, false, ngl::forward>>) & actor_gatewayc2g::handle
					);
					break;
				}
			}
		};
		
		// 二次转发   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		第一次转发		######	第二次转发	#######
		// 需要支持二次转发

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR>
		class register_recvforward2
		{
		public:
			template <typename T>
			static void func()
			{
				switch (xmlnode::m_nodetype)
				{
				case ngl::GAME:// gateway->game
					actor_role::register_recvforward<TYPE, actor_role>((Tfun<actor_role, T>) & actor_role::handle_forward<ACTOR, T>);
					break;
				case ngl::GATEWAY:// client->gateway
					actor_gatewayc2g::register_forward<TYPE, false, actor_gatewayc2g>(
						(Tfun<actor_gatewayc2g, np_actor_forward<T, TYPE, false, ngl::forward>>) & actor_gatewayc2g::handle
					);
					break;
				}
			}
		};

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