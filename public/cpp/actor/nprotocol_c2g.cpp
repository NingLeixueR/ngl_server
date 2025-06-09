#include "example.pb.h"
#include "net.pb.h"

#include "nforward.h"

namespace ngl
{
	// # 注册client到game的消息 
	// # 会注册gateway的转发与game的处理
	void nforward::c2g()
	{
		register_c2g<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_BAG_SYNC
			, pbnet::PROBUFF_NET_CHANGE_ANGLE
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_RECHARGE
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_SWITCH_LINE
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>();

		// actor_chat 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// actor_example_guess_number 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_GUESS_NUMBER
			, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
		>();

		// actor_example_manage 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_MANAGE
			, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
		>();

		// actor_example_match 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_MATCH
			, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
			, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN
			, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM
		>();

		// actor_family 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_FAMILY
			, pbnet::PROBUFF_NET_FAMIL_CEDE
			, pbnet::PROBUFF_NET_FAMIL_CHANGENAME
			, pbnet::PROBUFF_NET_FAMIL_CREATE
			, pbnet::PROBUFF_NET_FAMIL_INFO
			, pbnet::PROBUFF_NET_FAMIL_JOIN
			, pbnet::PROBUFF_NET_FAMIL_LEAVE
			, pbnet::PROBUFF_NET_FAMIL_LIST
			, pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN
			, pbnet::PROBUFF_NET_FAMIL_SIGN
		>();

		// actor_friends 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
			, pbnet::PROBUFF_NET_FRIEND_ADD
			, pbnet::PROBUFF_NET_FRIEND_ERASE
			, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD
		>();

		// actor_mail 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_DEL
			, pbnet::PROBUFF_NET_MAIL_DRAW
			, pbnet::PROBUFF_NET_MAIL_LIST
			, pbnet::PROBUFF_NET_MAIL_READ
		>();

		// actor_notice 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// actor_ranklist 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

	}
}// namespace ngl