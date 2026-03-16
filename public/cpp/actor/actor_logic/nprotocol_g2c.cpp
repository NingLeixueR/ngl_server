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
// File overview: Implements logic for actor logic.


#include "actor/actor_logic/nforward.h"
#include "actor/pb/example.pb.h"
#include "actor/pb/net.pb.h"

namespace ngl
{
	// # Registergametoclient message
	// # Registergateway forwardingandclient handle
	void nforward::g2c()
	{
		register_g2c<
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB,
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR,
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO,
			pbexample::PROBUFF_EXAMPLE_PLAY_CREATE,
			pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE,
			pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE,
			pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING,
			pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT,
			pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE,
			pbnet::PROBUFF_NET_BAG_UPDATE,
			pbnet::PROBUFF_NET_CHAT_RESPONSE,
			pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE,
			pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW,
			pbnet::PROBUFF_NET_ERROR,
			pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_RESPONSE,
			pbnet::PROBUFF_NET_GET_TIME_RESPONSE,
			pbnet::PROBUFF_NET_KCPSESSION_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_READ_RESPONSE,
			pbnet::PROBUFF_NET_MSG_RESPONSE,
			pbnet::PROBUFF_NET_NOTICE_RESPONSE,
			pbnet::PROBUFF_NET_RANKLIST_RESPONSE,
			pbnet::PROBUFF_NET_RECHARGE_RESPONSE,
			pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE,
			pbnet::PROBUFF_NET_ROLESTAT,
			pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE,
			pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE,
			pbnet::PROBUFF_NET_SYNC_ATTRIBUTE,
			pbnet::PROBUFF_NET_SYNC_POSITION,
			pbnet::PROBUFF_NET_SYNC_UNIT,
			pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE
		>();
	}
}//namespace ngl