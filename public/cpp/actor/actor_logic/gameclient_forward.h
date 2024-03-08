#pragma once

#include "actor_gatewayc2g.h"
#include "actor_gatewayg2c.h"
#include "actor_gateway.h"
#include "actor_robot.h"
#include "actor_role.h"
#include "actor.h"
#include "xmlnode.h"

namespace ngl
{
	class gameclient_forward
	{
		template <EPROTOCOL_TYPE TYPE, typename T>
		static void register_recvforward(const T* apdata)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::GAME:// gateway->game
				actor_role::register_recvforward<TYPE, actor_role>(dregister_fun_handle(actor_role, T));
				break;
			case ngl::GATEWAY:// client->gateway
				actor_gatewayc2g::register_forward<TYPE, false, actor_gatewayc2g>(
					(Tfun<actor_gatewayc2g, actor_forward<T, TYPE, false, ngl::forward>>)& actor_gatewayc2g::handle
				);
				break;
			}
		}

		template <EPROTOCOL_TYPE TYPE, typename T, typename ...ARG>
		static void register_recvforward(const T* apdata, const ARG*... arg)
		{
			gameclient_forward::register_recvforward<TYPE>(apdata);
			gameclient_forward::register_recvforward<TYPE>(arg...);
		}

		// 二次转发   [client]->[gateway]			->			[game]		->		[module]
		// ##############################		第一次转发		######	第二次转发	#######
		// 需要支持二次转发
		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename T>
		static void register_recvforward2(const T* apdata)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::GAME:// gateway->game
				actor_role::register_recvforward<TYPE, actor_role>((Tfun<actor_role, T>)& actor_role::handle_forward<ACTOR, T>);
				break;
			case ngl::GATEWAY:// client->gateway
				actor_gatewayc2g::register_forward<TYPE, false, actor_gatewayc2g>(
					(Tfun<actor_gatewayc2g, actor_forward<T, TYPE, false, ngl::forward>>) & actor_gatewayc2g::handle
				);
				break;
			}
		}

		template <EPROTOCOL_TYPE TYPE, ENUM_ACTOR ACTOR, typename T, typename ...ARG>
		static void register_recvforward2(const T* apdata, const ARG*... arg)
		{
			gameclient_forward::register_recvforward2<TYPE, ACTOR>(apdata);
			gameclient_forward::register_recvforward2<TYPE, ACTOR>(arg...);
		}

		template <EPROTOCOL_TYPE TYPE, typename T>
		static void register_forward(const T* ap)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::ROBOT:// gateway->client
				actor_robot::register_actor<TYPE, actor_robot>(false, dregister_fun_handle(actor_robot, T));
				break;
			case ngl::GATEWAY:// game->gateway
				actor_gatewayg2c::register_forward<TYPE, true, actor_gatewayg2c>(
					(Tfun<actor_gatewayg2c, actor_forward<T, TYPE, true, ngl::forward>>)& actor_gatewayg2c::handle
				);
				break;
			}
		}

		template <EPROTOCOL_TYPE TYPE, typename T, typename ...ARG>
		static void register_forward(const T* ap, const ARG*... arg)
		{
			gameclient_forward::register_forward<TYPE>(ap);
			gameclient_forward::register_forward<TYPE>(arg...);
		}

	public:
		//// ### client->gateway->game
		static void c2g();

		////// ### game->gateway->client
		static void g2c();
	};
}