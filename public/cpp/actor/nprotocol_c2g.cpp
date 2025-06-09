#include "example.pb.h"
#include "net.pb.h"

#include "nforward.h"

namespace ngl
{
	// # ע��client��game����Ϣ 
	// # ��ע��gateway��ת����game�Ĵ���
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

		// actor_chat ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// actor_example_guess_number ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_GUESS_NUMBER
			, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
		>();

		// actor_example_manage ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_MANAGE
			, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
		>();

		// actor_example_match ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_EXAMPLE_MATCH
			, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
			, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN
			, pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM
		>();

		// actor_family ģ�����ת��
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

		// actor_friends ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
			, pbnet::PROBUFF_NET_FRIEND_ADD
			, pbnet::PROBUFF_NET_FRIEND_ERASE
			, pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD
		>();

		// actor_mail ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_DEL
			, pbnet::PROBUFF_NET_MAIL_DRAW
			, pbnet::PROBUFF_NET_MAIL_LIST
			, pbnet::PROBUFF_NET_MAIL_READ
		>();

		// actor_notice ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// actor_ranklist ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

	}
}// namespace ngl