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
	class gateway_game_forward
	{
		template <EPROTOCOL_TYPE TYPE, typename T, typename ...ARG>
		static void register_recvforward(const T* apdata, const ARG*... arg)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::GAME:// gateway->game
				actor_role::register_recvforward<TYPE, actor_role>(apdata, arg...);
				break;
			case ngl::GATEWAY:// client->gateway
				actor_gateway_client2game::register_forward<TYPE, false>(&actor_gateway_client2game::getInstance(), apdata, arg...);
				break;
			}
		}

		template <EPROTOCOL_TYPE TYPE, typename T, typename ...ARG>
		static void register_forward(const T* ap, const ARG*... arg)
		{
			switch (xmlnode::m_nodetype)
			{
			case ngl::ROBOT:// gateway->client
				actor_robot::register_actor<TYPE, actor_robot>(false, ap, arg...);
				break;
			case ngl::GATEWAY:// game->gateway
				actor_gateway_game2client::register_forward<TYPE, true>(&actor_gateway_game2client::getInstance(), ap, arg...);
				break;
			}
		}

	public:
		


		//// ### client->gateway->game
		static void client2game();

		////// ### game->gateway->client
		static void game2client();
	};



}