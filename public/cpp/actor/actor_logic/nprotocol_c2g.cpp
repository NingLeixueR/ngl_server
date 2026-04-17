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
// File overview: Registers client-to-game forwarding handlers.


#include "actor/actor_logic/nforward.h"
#include "actor/generated/pb/example.pb.h"
#include "actor/generated/pb/net.pb.h"

namespace ngl
{
	// Register client-to-game message forwarding.
	// Sets up gateway forwarding handlers and game-side message handlers.
	void nforward::c2g()
	{
		register_c2g<
			pbnet::PROBUFF_NET_BAG_SYNC
			, pbnet::PROBUFF_NET_CHANGE_ANGLE
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_RECHARGE
			, pbnet::PROBUFF_NET_ROLE_CREATE
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_SWITCH_LINE
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>();

		// actor_chat module secondary forwarding
		register_c2g_2< ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// actor_example_guess_number module secondary forwarding
		register_c2g_2< ACTOR_EXAMPLE_GUESS_NUMBER
			, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
		>();

		// actor_example_manage module secondary forwarding
		register_c2g_2< ACTOR_EXAMPLE_MANAGE
			, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
		>();

		// actor_example_match module secondary forwarding
		register_c2g_2< ACTOR_EXAMPLE_MATCH
			, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
		>();

		// actor_family module secondary forwarding
		register_c2g_2< ACTOR_FAMILY
			, pbnet::PROBUFF_NET_FAMIL_CEDE
		>();

		// actor_friends module secondary forwarding
		register_c2g_2< ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
		>();

		// actor_mail module secondary forwarding
		register_c2g_2< ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// actor_notice module secondary forwarding
		register_c2g_2< ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// actor_ranklist module secondary forwarding
		register_c2g_2< ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

		// actor_testlua module secondary forwarding
		register_c2g_2< ACTOR_TESTLUA
			, pbnet::PROBUFF_NET_TESTLUA
		>();

	}
}// namespace ngl