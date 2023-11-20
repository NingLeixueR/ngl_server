#pragma once

#include "actor.h"
#include "actor_role.h"
#include "actor_gateway.h"
#include "xmlnode.h"
#include "actor_robot.h"
#include "actor_gateway_game2client.h"
#include "actor_gateway_client2game.h"

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
				actor_gateway_client2game::register_forward<TYPE, false, actor_gateway_client2game>(
					(Tfun<actor_gateway_client2game, actor_forward<T, TYPE, false, ngl::forward>>)& actor_gateway_client2game::handle
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

		template <EPROTOCOL_TYPE TYPE, typename T>
		static void register_forward(const T* ap)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::ROBOT:// gateway->client
				actor_robot::register_actor<TYPE, actor_robot>(false, dregister_fun_handle(actor_robot, T));
				break;
			case ngl::GATEWAY:// game->gateway
				actor_gateway_game2client::register_forward<TYPE, true, actor_gateway_game2client>(
					(Tfun<actor_gateway_game2client, actor_forward<T, TYPE, true, ngl::forward>>)& actor_gateway_game2client::handle
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